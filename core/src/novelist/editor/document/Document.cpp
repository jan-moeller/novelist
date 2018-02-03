/**********************************************************
 * @file   Document.cpp
 * @author jan
 * @date   1/25/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QtGui/QTextBlock>
#include <QDebug>
#include "editor/document/Document.h"
#include "editor/document/TextCursor.h"

namespace novelist::editor {
    Document::Document(gsl::not_null<TextFormatManager*> formatMgr, QString title,
            gsl::not_null<ProjectLanguage const*> lang)
            :m_properties(this, std::move(title), lang),
             m_formatMgr(formatMgr),
             m_doc(std::make_unique<QTextDocument>())
    {
        if (m_formatMgr->size() == 0)
            throw std::invalid_argument("TextFormatManager may not be empty.");

        m_doc->setIndentWidth(8);
        m_doc->setUndoRedoEnabled(false); // Disable internal undo, we do it ourselves.

        connect(m_doc.get(), &QTextDocument::blockCountChanged, this, &Document::onBlockCountChanged);

        // Per default, use the first format
        TextCursor cursor(this);
        cursor.setParagraphFormat(formatMgr->idFromIndex(0));
        cursor.setCharacterFormat(formatMgr->idFromIndex(0));
    }

    QUndoStack& Document::undoStack() noexcept
    {
        return m_undoStack;
    }

    Properties& Document::properties() noexcept
    {
        return m_properties;
    }

    Properties const& Document::properties() const noexcept
    {
        return m_properties;
    }

    QString Document::toRawText() const noexcept
    {
        return m_doc->toRawText();
    }

    bool Document::empty() const noexcept
    {
        return m_doc->isEmpty();
    }

    void Document::onParagraphFormatChanged(int blockIdx) noexcept
    {
        // Auto text indent logic
        if (blockIdx == 0 || blockIdx >= m_doc->blockCount())
            return;

        auto block = m_doc->findBlockByNumber(blockIdx);
        updateParagraphLayout(block);
    }

    void Document::onBlockCountChanged(int /*newBlockCount*/) noexcept
    {
        for (QTextBlock b = m_doc->firstBlock(); b.isValid(); b = b.next())
            updateParagraphLayout(b);
    }

    void Document::updateParagraphLayout(QTextBlock block) noexcept
    {
        QTextBlock thisBlock = block;
        QTextBlock prevBlock = block.previous();

        if (!thisBlock.isValid()) {
            qWarning() << "Tried to update invalid paragraph";
            return;
        }

        auto thisFormatId = m_formatMgr->getIdOfBlockFormat(thisBlock.blockFormat());
        auto thisFormat = m_formatMgr->getTextFormat(m_formatMgr->indexFromId(thisFormatId));
        auto thisBlockFormat = *m_formatMgr->getTextBlockFormat(thisFormatId);
        auto thisBlockCharFormat = *m_formatMgr->getTextCharFormat(thisFormatId);

        if (prevBlock.isValid()) {
            auto prevFormatId = m_formatMgr->getIdOfBlockFormat(prevBlock.blockFormat());
            auto prevFormat = m_formatMgr->getTextFormat(m_formatMgr->indexFromId(prevFormatId));

            if (m_formatMgr->checkNeedAutoTextIndent(prevFormat, thisFormat))
                thisBlockFormat.setTextIndent(thisBlockFormat.indent() + m_doc->indentWidth());
        }

        QTextCursor cursor(m_doc.get());
        cursor.joinPreviousEditBlock();
        cursor.setPosition(thisBlock.position());
        cursor.setBlockFormat(thisBlockFormat);
        cursor.setBlockCharFormat(thisBlockCharFormat);
        cursor.endEditBlock();
    }

    namespace internal {
        TextInsertCommand::TextInsertCommand(Document* doc, int pos, QString added) noexcept
                :m_doc(doc),
                 m_pos(pos),
                 m_added(std::move(added))
        {
            setText(Document::tr("text insertion in %1").arg(doc->properties().title()));
        }

        void TextInsertCommand::undo()
        {
            QTextCursor cursor(m_doc->m_doc.get());
            cursor.setPosition(m_pos + m_added.size());
            cursor.setPosition(m_pos, QTextCursor::KeepAnchor);
            cursor.removeSelectedText();
        }

        void TextInsertCommand::redo()
        {
            QTextCursor cursor(m_doc->m_doc.get());
            cursor.setPosition(m_pos);
            cursor.insertText(m_added);
        }

        int TextInsertCommand::id() const
        {
            return 0;
        }

        bool TextInsertCommand::mergeWith(const QUndoCommand* other)
        {
            auto cmd = dynamic_cast<TextInsertCommand const*>(other);
            if (!cmd)
                return false;
            if (m_pos + m_added.size() == cmd->m_pos && !isCompleteSentence(m_added, &m_doc->m_properties.language())) {
                m_added += cmd->m_added;
                return true;
            }
            return false;
        }

        TextRemoveCommand::TextRemoveCommand(Document* doc, int pos, QString removed) noexcept
        : m_doc(doc),
          m_pos(pos),
          m_removed(std::move(removed))
        {
            setText(Document::tr("text removal in %1").arg(doc->properties().title()));
        }

        void TextRemoveCommand::undo()
        {
            QTextCursor cursor(m_doc->m_doc.get());
            cursor.setPosition(m_pos);
            cursor.insertText(m_removed);
        }

        void TextRemoveCommand::redo()
        {
            QTextCursor cursor(m_doc->m_doc.get());
            cursor.setPosition(m_pos + m_removed.size());
            cursor.setPosition(m_pos, QTextCursor::KeepAnchor);
            cursor.removeSelectedText();
        }

        int TextRemoveCommand::id() const
        {
            return 1;
        }

        bool TextRemoveCommand::mergeWith(QUndoCommand const* other)
        {
            auto cmd = dynamic_cast<TextRemoveCommand const*>(other);
            if (!cmd)
                return false;
            if (cmd->m_pos == m_pos && !isCompleteSentence(m_removed, &m_doc->m_properties.language())) {
                m_removed += cmd->m_removed;
                return true;
            }
            if (cmd->m_pos + cmd->m_removed.size() == m_pos && !isCompleteSentence(cmd->m_removed, &m_doc->m_properties.language())) {
                m_pos = cmd->m_pos;
                m_removed = cmd->m_removed + m_removed;
                return true;
            }
            return false;
        }

        BlockInsertCommand::BlockInsertCommand(Document* doc, int pos) noexcept
                :m_doc(doc),
                 m_pos(pos)
        {
            setText(Document::tr("new paragraph in %1").arg(doc->properties().title()));
        }

        void BlockInsertCommand::undo()
        {
            QTextCursor cursor(m_doc->m_doc.get());
            cursor.setPosition(m_pos);
            cursor.deleteChar();
        }

        void BlockInsertCommand::redo()
        {
            QTextCursor cursor(m_doc->m_doc.get());
            cursor.setPosition(m_pos);
            cursor.insertBlock();
        }
    }
}