#ifndef MYEDITABLEROWMODEL_H
#define MYEDITABLEROWMODEL_H

#include <QStandardItemModel>

class MyEditableRowModel : public QStandardItemModel {
    Q_OBJECT

public:
    // 构造函数
    MyEditableRowModel(int rows, int columns, QObject *parent = nullptr);
    MyEditableRowModel(QObject *parent = nullptr);
    // 设置某一行可编辑
    void setEditableRow(int row);

    // 清除可编辑行（让所有行都不可编辑）
    void clearEditableRow();

private:
    // 重写 flags 方法，控制某一行是否可编辑
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    int m_editableRow;  // 当前可编辑的行号，-1 表示没有可编辑的行
};

#endif // MYEDITABLEROWMODEL_H
