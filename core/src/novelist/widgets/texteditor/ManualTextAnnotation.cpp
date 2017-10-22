/**********************************************************
 * @file   ManualTextAnnotation.cpp
 * @author jan
 * @date   10/20/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "windows/NoteEditWindow.h"
#include "widgets/texteditor/ManualTextAnnotation.h"

namespace novelist {
    ManualTextAnnotation::ManualTextAnnotation(gsl::not_null<QTextDocument*> doc, int left, int right, QString msg)
            :TextAnnotation(doc, left, right, std::move(msg))
    {
        connect(&m_editAction, &QAction::triggered, this, &ManualTextAnnotation::onEditAction);
        connect(&m_removeAction, &QAction::triggered, this, &ManualTextAnnotation::onRemoveAction);

        m_editAction.setText(tr("Edit"));
        m_removeAction.setText(tr("Remove"));

        m_menu.setTitle(tr("Note"));
        m_menu.addAction(&m_editAction);
        m_menu.addAction(&m_removeAction);
    }

    QMenu const& ManualTextAnnotation::menu() const noexcept
    {
        return m_menu;
    }

    void ManualTextAnnotation::onEditAction()
    {
        NoteEditWindow wnd;
        wnd.setText(message());
        auto result = wnd.exec();
        if(result == QDialog::Accepted)
            setMessage(wnd.text());
    }

    void ManualTextAnnotation::onRemoveAction()
    {
        setRange(0, 0); // This will trigger the collapsed() signal and the annotation will be removed
    }
}