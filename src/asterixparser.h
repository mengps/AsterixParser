#ifndef ASTERIXPARSER_H
#define ASTERIXPARSER_H

#include "asterixparser_global.h"

#include <QObject>
#include <QVariant>

/**
 * @brief The SimpleAsterixRecordBlock struct
 */
struct SimpleAsterixRecordBlock
{
    /*! [字段引用编号] */
    int frn;
    /*! [数据项ID,例如(I062/070)] */
    QString id;
    /*! [数据项名称] */
    QString name;
    /*! [数据项原始值] */
    QByteArray rawValue;
    /*! [数据项刻度] */
    qreal scale;
    /*! [数据项单位] */
    QString unit;
    /*! [数据项实际值] */
    QVariant value;
    /*! [子数据块列表] */
    QList<SimpleAsterixRecordBlock> subBlock;
};

/**
 * @brief The SimpleReservedExpansionField struct
 */
struct SimpleReservedExpansionField
{
    struct SubField {
        /*! [字段名称] */
        QString name;
        /*! [字段原始值] */
        QByteArray value;
    };
    /*! [字段类型] */
    quint8 type = 0;
    /*! [子字段列表] */
    QList<SubField> subField;
};

QT_FORWARD_DECLARE_CLASS(AsterixParserPrivate);

class ASTERIXPARSER_EXPORT AsterixParser : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief AsterixParser 构造
     * @param specificationDir Asterix规范文件目录[xml格式]
     * @param parent
     */
    AsterixParser(const QString &specificationDir, QObject *parent = nullptr);
    ~AsterixParser();

    /**
     * @brief getCategory 获取类别
     * @param asterixData Asterix数据包
     * @return int
     */
    int getCategory(const uchar *asterixData);

    /**
     * @brief getU8 字节转U8
     * @param data 原始字节
     * @return quint8
     */
    quint8 getU8(const QByteArray &data);

    /**
     * @brief getU16 字节转U16
     * @param data 原始字节
     * @return quint16
     */
    quint16 getU16(const QByteArray &data);

    /**
     * @brief getU32 字节转U32
     * @param data 原始字节
     * @return quint32
     */
    quint32 getU32(const QByteArray &data);

    /**
     * @brief parserToFsnMap 解析为{fsn, block}映射
     * @param asterixData Asterix数据包
     * @return QMap<int, SimpleAsterixRecordBlock>
     */
    QMap<int, SimpleAsterixRecordBlock> parserToFsnMap(const uchar *asterixData);

    /**
     * @brief parserToIdMap 解析为{id, block}映射
     * @param asterixData Asterix数据包
     * @return QMap<int, SimpleAsterixRecordBlock>
     */
    QMap<QString, SimpleAsterixRecordBlock> parserToIdMap(const uchar *asterixData);

    /**
     * @brief parserReservedExpansionField 解析保留扩展字段
     * @warning 目前仅实现[cat021]
     * @param cat 类别
     * @param ref 扩展字段记录块
     * @return QMap<int, SimpleReservedExpansionField>
     */
    QMap<int, SimpleReservedExpansionField> parserReservedExpansionField(int cat, const SimpleAsterixRecordBlock &ref);

private:
    QScopedPointer<AsterixParserPrivate> d_ptr;
    Q_DECLARE_PRIVATE(AsterixParser);
};

#endif // ASTERIXPARSER_H
