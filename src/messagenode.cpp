#include "messagenode.h"
#include "ui_messagenode.h"

MessageNode::MessageNode(QObject* parent) : QStyledItemDelegate(parent) {
}

MessageNode::MessageNode(QString message, QObject* parent) : QStyledItemDelegate(parent), m_message(message) {
}
void MessageNode::paint(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QRect r = option.rect;

    //Color: #C4C4C4
    QPen linePen(QColor::fromRgb(211,211,211), 1, Qt::SolidLine);

    //Color: #005A83
    QPen lineMarkedPen(QColor::fromRgb(0,90,131), 1, Qt::SolidLine);

    //Color: #333
    QPen fontPen(QColor::fromRgb(51,51,51), 1, Qt::SolidLine);

    //Color: #fff
    QPen fontMarkedPen(Qt::white, 1, Qt::SolidLine);

    if (option.state & QStyle::State_Selected) {
        QColor color(0,120,174,128);
        painter->setBrush(color);
        painter->drawRect(option.rect);

        //BORDER
//        painter->setPen(fontMarkedPen);
        painter->drawLine(r.topLeft(),r.topRight());
        painter->drawLine(r.topRight(),r.bottomRight());
        painter->drawLine(r.topLeft(),r.bottomLeft());

//        painter->setPen(lineMarkedPen);
        painter->drawLine(r.bottomLeft(),r.bottomRight());

    }
    else {
        //BACKGROUND
                    //ALTERNATING COLORS
        painter->setBrush( (index.row() % 2) ? Qt::white : QColor(252,252,252) );
        painter->drawRect(r);

        //BORDER
       painter->setPen(fontMarkedPen);
       painter->drawLine(r.topLeft(),r.topRight());
       painter->drawLine(r.topRight(),r.bottomRight());
       painter->drawLine(r.topLeft(),r.bottomLeft());

//        painter->setPen(lineMarkedPen);
//        painter->drawLine(r.bottomLeft(),r.bottomRight());
    }

    //GET TITLE
    QString message = index.data(Qt::DisplayRole).toString();
    //TITLE

    painter->drawRect(r.adjusted(0, 0, -painter->pen().width(), -painter->pen().width()));
    QRect boundedrect;
    QPen pen(Qt::black,Qt::DashLine);
    painter->setPen(pen);
    r .adjust(5, -5, -5, 0);
    painter->setPen(fontPen);
    painter->setFont(QFont("AnjaliOldLipi", 14, QFont::Normal));

    painter->drawText(r, Qt::AlignBottom|Qt::AlignRight|Qt::TextWrapAnywhere, message, &boundedrect);

    //Draws a rectangle over the text. Good for debugging the bounding rectangle around the text. Covers the text
//    QPen pen2(Qt::transparent,Qt::DotLine);
//    painter->setPen(pen2);
//    painter->drawRect(boundedrect.adjusted(0, 0, -painter->pen().width(), -painter->pen().width()));
}

QSize MessageNode::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QFont font("AnjaliOldLipi",14);
    QFontMetrics metric(font);
    QRect bounding(0,0,option.rect.width(),metric.size(Qt::AlignBottom|Qt::AlignRight|Qt::TextWrapAnywhere,index.data(Qt::DisplayRole).toString()).height());
    bounding = metric.boundingRect(bounding,Qt::AlignBottom|Qt::AlignRight|Qt::TextWrapAnywhere,index.data(Qt::DisplayRole).toString());
    bounding.adjust(0,0,0,10);
    return bounding.size();
}
