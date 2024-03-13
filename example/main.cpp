#include <QCoreApplication>
#include <QDebug>
#include <QtEndian>

#include "asterixparser.h"

QString applyUnitAndScale(const QVariant &value, qreal scale, const QString &unit)
{
    if (qFuzzyCompare(scale, 1))
        return QString::number(value.toDouble()) + (unit.isEmpty() ? "" : (" "  + unit));
    else
        return QString::number(value.toDouble() * scale, 'f', 10) + (unit.isEmpty() ? "" : (" "  + unit));
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    //cat021
    uchar test[] = {
          0x15, 0x00, 0x35, 0xcb, 0x19, 0x71
        , 0x11, 0xc1, 0x01, 0x04, 0x16, 0x00, 0x11, 0x44, 0x4c, 0x65, 0x80, 0x09, 0xf1, 0x80, 0x2c, 0x25
        , 0xd8, 0x59, 0xe5, 0xff, 0xe0, 0x07, 0x4c, 0x65, 0x80, 0x02, 0x7b, 0x2d, 0x35, 0x08, 0x12, 0x00
        , 0x03, 0x34, 0x81, 0x37, 0xcf, 0x5d, 0xa0, 0x01, 0x07, 0x88, 0x10, 0x01, 0x11, 0x11, 0x02
    };

    //cat062
    /*uchar test[] = {
        0x3e, 0x00, 0x2b, 0x19, 0x31, 0x10, 0x47, 0x88, 0xf6, 0x00, 0x56, 0xfe, 0x34, 0x01, 0x27, 0xad,
        0x07, 0x00, 0x60, 0x6c, 0x31, 0x00, 0x00, 0x00, 0xc1, 0x01, 0x32, 0xff, 0xe1, 0x01, 0x60, 0x6c,
        0x31, 0x00, 0x00, 0x00, 0x4e, 0xee, 0x00, 0x93, 0x00, 0x00, 0x00
    };*/

    AsterixParser parser(QT_STRINGIFY(PWD_PATH) + QString("/../asterixSpecification"));

    auto map = parser.parseToFsnMap(test);

    for (const auto &block: map) {
        qDebug() << block.frn << block.id << block.name << block.rawValue;
        if (!block.subBlock.isEmpty()) {
            for (const auto &subBlock: block.subBlock)
                qDebug() << "  "
                         << subBlock.frn
                         << subBlock.id
                         << subBlock.name
                         << subBlock.value
                         << applyUnitAndScale(subBlock.value, subBlock.scale, subBlock.unit);
        }
    }

    auto ref_map = parser.parseReservedExpansionField(parser.getCategory(test), map[48]);
    for (const auto &ref: ref_map) {
        for (const auto &subField: ref.subField)
            qDebug() << "  "
                     << subField.name
                     << (subField.value.size() == 1 ? (parser.getU8(subField.value)) : (parser.getU16(subField.value)));
    }


    return app.exec();
}
