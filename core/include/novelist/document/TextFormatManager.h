/**********************************************************
 * @file   TextFormatManager.h
 * @author jan
 * @date   1/19/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_TEXTFORMATMANAGER_H
#define NOVELIST_TEXTFORMATMANAGER_H

#include <vector>
#include <QtCore/QString>
#include <QtGui/QTextBlockFormat>
#include <novelist_core_export.h>
#include "util/StrongTypedef.h"
#include "util/Identity.h"

namespace novelist {
    class NOVELIST_CORE_EXPORT TextFormatManager {
    public:
        //!< Type of all format ids
        STRONG_TYPEDEF(FormatId, uint32_t);

        /**
         * Paragraph alignment
         */
        enum class Alignment {
            Left,
            Right,
            Center,
            Fill,
        };

        /**
         * Paragraph margin
         */
        struct Margin {
            uint32_t m_left = 0;
            uint32_t m_right = 0;
            uint32_t m_top = 0;
            uint32_t m_bottom = 0;
        };

        /**
         * Paragraph indentation
         */
        struct Indentation {
            uint32_t m_indent = 0;
            uint32_t m_textIndent = 0;
            bool m_autoTextIndent = false;
        };

        /**
         * Character format
         */
        struct CharacterFormat {
            bool m_bold = false;
            bool m_italic = false;
            bool m_underline = false;
            bool m_overline = false;
            bool m_strikethrough = false;
            bool m_smallCaps = false;
        };

        /**
         * Text format
         */
        struct TextFormat {
            QString m_name{};
            Alignment m_alignment = Alignment::Left;
            Margin m_margin{};
            Indentation m_indentation{};
            CharacterFormat m_characterFormat{};
        };

        /**
         * Insert a new text format into the manager. It will be added after the last element.
         * @param textFormat New text format
         * @return ID of the new format
         */
        FormatId push_back(TextFormat textFormat) noexcept;

        /**
         * Insert a new text format at a specific position into the manager.
         * @param idx Index to add new format at
         * @param textFormat New text format
         * @return ID of the new format
         */
        FormatId insert(size_t idx, TextFormat textFormat) noexcept;

        /**
         * Remove text format with a given ID
         * @param id ID of the format
         * @return true in case the format was removed, false with ID is invalid
         */
        bool remove(FormatId id) noexcept;

        /**
         * Change order of formats, moving the format at \p srcIdx to \p destIdx
         * @param srcIdx Source index
         * @param destIdx Destination index
         */
        void move(size_t srcIdx, size_t destIdx) noexcept;

        /**
         * Gets the format ID of the element a certain index
         * @param idx Index
         * @return Format ID of index. Invalid if index was out of range.
         */
        FormatId idFromIndex(size_t idx) const noexcept;

        /**
         * Gets the index of the element with a certain id
         * @param id ID
         * @return Index of element with requested id. Out of bounds if id was invalid.
         */
        size_t indexFromId(FormatId id) const noexcept;

        /**
         * Retrieve a pointer to the text format at a certain index
         * @param id ID of the format
         * @return Pointer to the format if found, or nullptr if index is out of bounds
         */
        TextFormat const* getTextFormat(size_t idx) const noexcept;

        /**
         * Retrieve a pointer to the text format associated with a given ID
         * @param id ID of the format
         * @return Pointer to the format if found, or nullptr if ID is invalid
         */
        TextFormat const* getTextFormat(FormatId id) const noexcept;

        /**
         * Replaces a text format.
         * @param id ID of the format
         * @param format New format
         */
        void setTextFormat(FormatId id, TextFormat format) noexcept;

        /**
         * Retrieve a pointer to the text block format associated with a given ID
         * @param id ID of the format
         * @return Pointer to the block format if found or nullptr if ID is invalid
         */
        QTextBlockFormat const* getTextBlockFormat(FormatId id) const noexcept;

        /**
         * Retrieve a pointer to the text character format associated with a given ID
         * @param id ID of the format
         * @return Pointer to the character format if found or nullptr if ID is invalid
         */
        QTextCharFormat const* getTextCharFormat(FormatId id) const noexcept;

        /**
         * @return Amount of formats currently managed
         */
        size_t size() const noexcept;

    private:
        using IdType = IdManager<TextFormatManager>::IdType;
        struct InternalTextFormat {
            IdType m_id;
            TextFormat m_textFormat{};
            QTextBlockFormat m_blockFormat{};
            QTextCharFormat m_charFormat{};
        };

        QTextBlockFormat extractBlockFormat(TextFormat const& format, uint32_t id) const noexcept;

        QTextCharFormat extractCharFormat(TextFormat const& format, uint32_t id) const noexcept;

        Qt::Alignment convertAlignment(Alignment align) const noexcept;

        IdManager<TextFormatManager> m_idMgr;
        std::vector<InternalTextFormat> m_formats;
        static int const s_typePropertyId = QTextFormat::UserProperty + 1;
    };
}

Q_DECLARE_METATYPE(novelist::TextFormatManager::Alignment*)

Q_DECLARE_METATYPE(novelist::TextFormatManager::Margin*)

Q_DECLARE_METATYPE(novelist::TextFormatManager::Indentation*)

Q_DECLARE_METATYPE(novelist::TextFormatManager::CharacterFormat*)

Q_DECLARE_METATYPE(novelist::TextFormatManager::TextFormat*)

#endif //NOVELIST_TEXTFORMATMANAGER_H