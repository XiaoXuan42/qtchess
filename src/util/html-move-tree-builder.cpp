#include "util/html-move-tree-builder.hpp"
#include "settings.hpp"

static QString styleSheet = R"(
    <style>
        body {
            background-color: %5;
            font-family: monospace;
        }

        .TreeMoveNumber {
            color: %6;
            font-weight: bold;
        }

        .TreeBody {
            font-size: 14px;
            line-height: 140%;
            font-family: monospace;
        }

        .TreeMove {
            text-decoration: none;
            color: %1;
        }

        ul.TreeVariant {
            font-size: 12px;
            color: %2;
            padding-left: 15px;
            list-style-type: none;
        }

        li.TreeVariant { }

        .TreeCurrentMove {
            font-weight: bold;
            background-color: %3;
            color: %4;
        }
    </style>
)";

HtmlMoveTreeBuilder& HtmlMoveTreeBuilder::addMoveNumber(const QString& number)
{
    m_html.append(QString("<span class='TreeMoveNumber'>%1</span>").arg(number));
    return *this;
}

HtmlMoveTreeBuilder& HtmlMoveTreeBuilder::addMove(const QString& move, size_t uid,
                                                  bool isCurrentMove)
{
    QString moveFormatted;

    if (isCurrentMove)
        moveFormatted = QString("<span class='TreeCurrentMove'>%1</span>").arg(move);
    else
        moveFormatted = move;

    m_html.append(QString("<a class='TreeMove' href='%1'>%2</a> ")
          .arg(QString::number(uid), moveFormatted));
    return *this;
}

HtmlMoveTreeBuilder& HtmlMoveTreeBuilder::addVariant(const QString& variant)
{
    static QString variantLiFmt = "<li class='TreeVariant'>( %1)</li>";
    static QString variantUlFmt = "<ul class='TreeVariant'>%1</ul>";

    m_html.append(variantUlFmt.arg(variantLiFmt.arg(variant)));
    return *this;
}

QString HtmlMoveTreeBuilder::html() const
{
    return m_html;
}

QString HtmlMoveTreeBuilder::htmlWithStyle() const
{
    QString style = styleSheet.arg(
        Settings::instance().get(Settings::PgnMoveColor).value<QColor>().name(),
        Settings::instance().get(Settings::PgnVariationColor).value<QColor>().name(),
        Settings::instance().get(Settings::PgnHiColor).value<QColor>().name(),
        Settings::instance().get(Settings::PgnHiMoveColor).value<QColor>().name(),
        Settings::instance().get(Settings::PgnBackgroundColor).value<QColor>().name(),
        Settings::instance().get(Settings::PgnNumberColor).value<QColor>().name()
    );
    return style + QString("<div class='TreeBody'>%1</div>").arg(m_html);
}

bool HtmlMoveTreeBuilder::isEmpty() const
{
    return m_html.isEmpty();
}
