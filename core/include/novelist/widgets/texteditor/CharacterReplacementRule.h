/**********************************************************
 * @file   CharacterReplacementRule.h
 * @author jan
 * @date   12/10/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_CHARACTERREPLACEMENTRULE_H
#define NOVELIST_CHARACTERREPLACEMENTRULE_H

#include <QtCore/QString>

namespace novelist {
    /**
     * Holds information on what character to replace with what other (pair of) character(s) when typing it into a
     * TextEditor. For example, when typing "'", replace it with "‘’" or "(" with "()".
     */
    struct CharacterReplacementRule {
        QString m_startChar; // Keyboard character that signifies the start, e.g. "("
        QString m_endChar; // Keyboard character that signifies the end, e.g. ")"
        QString m_replaceStartChar; // Replacement character for the keyboard start character
        QString m_replaceEndChar; // Replacement character for the keyboard end character
        QString m_requirementsRegExp = ""; // Must match the current paragraph to enable rule
        int m_replaceCaptureGroupNo = -1; // Replace the contents of a capture group
        Qt::KeyboardModifier m_enableKey = Qt::NoModifier; // Modifier key to enable this rule on the fly
    };
}

#endif //NOVELIST_CHARACTERREPLACEMENTRULE_H
