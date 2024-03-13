#include "asterixparser.h"
#include "asterix.h"
#include "bit.h"
#include "uap.h"

static SimpleReservedExpansionField __cat021_parserOneReservedExpansionField(QByteArray &rawValue, int type)
{
    enum class Ref_Type {
        BPS  = 1 << 7,
        SelH = 1 << 6,
        NAV  = 1 << 5,
        GAO  = 1 << 4,
        SGV  = 1 << 3,
        STA  = 1 << 2,
        TNH  = 1 << 1,
        MES  = 1 << 0,
        None = 0
    };

    SimpleReservedExpansionField sref;
    sref.type = type;

    switch (Ref_Type(type)) {
    case Ref_Type::BPS:
    {
        SimpleReservedExpansionField::SubField subField;
        if (rawValue.size() >= 2) {
            sref.subField.append({ "Barometric Pressure Setting", rawValue.left(2) });
            rawValue.remove(0, 2);
        }
    } break;
    case Ref_Type::SelH:
    {
        if (rawValue.size() >= 2) {
            sref.subField.append({ "Selected Heading", rawValue.left(2) });
            rawValue.remove(0, 2);
        }
    } break;
    case Ref_Type::NAV:
    {
        if (rawValue.size() >= 1) {
            sref.subField.append({ "Navigation Mode", rawValue.left(1) });
            rawValue.remove(0, 1);
        }
    } break;
    case Ref_Type::GAO:
    {
        if (rawValue.size() >= 1) {
            sref.subField.append({ "GPS Antenna Offset", rawValue.left(1) });
            rawValue.remove(0, 1);
        }
    } break;
    case Ref_Type::SGV:
    {
        bool hasFX = false;
        if (rawValue.size() >= 2) {
            auto value = rawValue.left(2);
            sref.subField.append({ "Surface Ground Vector", value });
            rawValue.remove(0, 2);
            hasFX = bool(getU16((uchar *)value.data(), 0) & 0x1);
        }
        /*! first FX bit */
        if (hasFX && rawValue.size() >= 1) {
            sref.subField.append({ "Surface Ground Vector First Extension", rawValue.left(1) });
            rawValue.remove(0, 1);
        }
    } break;
    case Ref_Type::STA:
    {
        bool hasFX = false;
        if (rawValue.size() >= 1) {
            auto value = rawValue.left(1);
            sref.subField.append({ "Aircraft Status", value });
            rawValue.remove(0, 1);
            hasFX = bool(getU16((uchar *)value.data(), 2) & 0x1);
        }
        /*! first FX bit */
        if (hasFX && rawValue.size() >= 1) {
            sref.subField.append({ "Aircraft Status Subfield", rawValue.left(1) });
            rawValue.remove(0, 1);
        }
    } break;
    case Ref_Type::TNH:
    {
        if (rawValue.size() >= 2) {
            sref.subField.append({ "True North Heading", rawValue.left(2) });
            rawValue.remove(0, 2);
        }
    } break;
    case Ref_Type::MES:
    {
        if (rawValue.size() >= 1) {
            sref.subField.append({ "Military Extended Squitter", rawValue.left(1) });
            rawValue.remove(0, 1);
        }
        sref.subField.append({ "Military Extended Squitter Subfield", rawValue });
        rawValue.clear();
    } break;
    default:
        break;
    }

    return sref;
}



static SimpleAsterixRecordBlock parserAsterixRecordBlock(const AsterixDataItem &dataItem)
{
    auto uapDataItem = *dataItem.uapDataItem();
    auto frn = uapDataItem.frn();
    const int ofs = uapDataItem.format() == UapDataItem::REPETIVE ? 1 : 0;

    SimpleAsterixRecordBlock block;
    block.frn = frn;
    block.id = uapDataItem.id();
    block.name = uapDataItem.name();
    block.rawValue = QByteArray((char *)dataItem.data(), dataItem.length());

    if (uapDataItem.format() == UapDataItem::COMPOUND) {
        for (int i = 0; i < dataItem.numSubfields(); i++) {
            auto subField = dataItem.subField(i);
            auto uapDataItem = *subField.uapDataItem();
            block.subBlock.append(parserAsterixRecordBlock(subField).subBlock);
        }
    } else {
        for (const UapField &uapField: qAsConst(uapDataItem.m_fields)) {
            if (!uapDataItem.fieldPresent(dataItem.data(), uapField))
                continue;

            const int correctedLength = uapDataItem.format() == UapDataItem::VARIABLE ? 1 : uapDataItem.length();
            QByteArray field = dataItem.bitfield(uapField.octed() + ofs, correctedLength, uapField.msb() - 1, uapField.lsb() - 1);
            QVariant value;

            switch (uapField.format())
            {
            case UapField::ICAO6STR:
            {
                value = AsterixDataItem::decodeIcaoStr((const uchar*)field.constData());
            } break;
            case UapField::UINTEGER:
            {
                value = bitfieldToUInt(field);
            } break;
            case UapField::INTEGER:
            {
                value = bitfieldToInt(field, uapField.msb() - 1 - (((uapField.lsb() - 1) / 8) * 8));
            } break;
            case UapField::OCTAL:
            {
                auto v = bitfieldToUInt(field);
                value = QString("0%1").arg(v, 0, 8);
            } break;
            case UapField::ASCII:
            {
                value = QString::fromLatin1(field);
            } break;
            case UapField::HEX:
            default:
            {
                value = bitfieldToUInt(field);
                QString str = "0x";
                for (int i=0; i < field.size(); i++)
                    str += QString("%1").arg((uint)((uchar)field[i]), 2, 16, QChar('0'));
            } break;
            }

            SimpleAsterixRecordBlock subBlock;
            subBlock.id = uapDataItem.id();
            subBlock.name = uapField.name();
            subBlock.frn = frn;
            subBlock.rawValue = field;
            subBlock.value = value;
            subBlock.scale = uapField.scale();
            if (uapField.unit() != UapField::UNDEFINED) {
                subBlock.unit = uapField.unitStr(uapField.unit());
            }

            block.subBlock.append(subBlock);
        }
    }

    return block;
}

class AsterixParserPrivate
{
public:
    Uap *m_uap = nullptr;
};

AsterixParser::AsterixParser(const QString &specificationDir, QObject *parent)
    : QObject{parent}
    , d_ptr(new AsterixParserPrivate)
{
    Q_D(AsterixParser);

    d->m_uap = new Uap;
    d->m_uap->readXml(specificationDir);
}

AsterixParser::~AsterixParser()
{

}

int AsterixParser::getCategory(const uchar *asterixData)
{
    Q_D(AsterixParser);

    auto key = *(quint8*)(asterixData + 0);

    if (d->m_uap->selectedUapCategory(key)) {
        return d->m_uap->selectedUapCategory(key)->category();
    }

    return key;
}

quint8 AsterixParser::getU8(const QByteArray &data)
{
    return ::getU8((uchar *)(data.data()), 0);
}

quint16 AsterixParser::getU16(const QByteArray &data)
{
    return ::getU16((uchar *)(data.data()), 0);
}

quint32 AsterixParser::getU32(const QByteArray &data)
{
    return ::getU32((uchar *)(data.data()), 0);
}

QMap<int, SimpleAsterixRecordBlock> AsterixParser::parserToFsnMap(const uchar *asterixData)
{
    Q_D(AsterixParser);

    AsterixBlock asterixBlock(asterixData, *d->m_uap);

    QMap<int, SimpleAsterixRecordBlock> map;

    for (int i = 0; i < asterixBlock.numRecords(); i++) {
        auto record = asterixBlock.record(i);
        for (int ii = 0; ii < record.numFields(); ii++) {
            auto block = parserAsterixRecordBlock(record.field(ii));
            map[block.frn] = block;
        }
    }

    return map;
}

QMap<QString, SimpleAsterixRecordBlock> AsterixParser::parserToIdMap(const uchar *asterixData)
{
    Q_D(AsterixParser);

    AsterixBlock asterixBlock(asterixData, *d->m_uap);

    QMap<QString, SimpleAsterixRecordBlock> map;

    for (int i = 0; i < asterixBlock.numRecords(); i++) {
        auto record = asterixBlock.record(i);
        for (int ii = 0; ii < record.numFields(); ii++) {
            auto block = parserAsterixRecordBlock(record.field(ii));
            map[block.id] = block;
        }
    }

    return map;
}

QMap<int, SimpleReservedExpansionField> AsterixParser::parserReservedExpansionField(int cat, const SimpleAsterixRecordBlock &ref)
{
    QMap<int, SimpleReservedExpansionField> map;
    auto rawValue = ref.rawValue;
    if (rawValue.size() >= 2) {
        auto len = ::getU8((uchar *)rawValue.data(), 0);
        if (rawValue.size() >= len) {
            rawValue = rawValue.mid(0, len);
            auto spec = ::getU8((uchar *)rawValue.data(), 1);
            rawValue.remove(0, 2);
            switch (cat) {
            case 21:
            {
                for (int i = 7; i >= 0; i--) {
                    quint8 type = 1 << i;
                    if (spec & type) {
                        map[type] = __cat021_parserOneReservedExpansionField(rawValue, type);
                    }
                    if (rawValue.isEmpty()) break;
                }
            } break;
            default:
                break;
            }
        }
    }

    return map;
}
