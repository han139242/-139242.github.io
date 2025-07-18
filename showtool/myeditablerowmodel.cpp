#include "MyEditableRowModel.h"

// 构造函数
MyEditableRowModel::MyEditableRowModel(int rows, int columns, QObject *parent)
    : QStandardItemModel(rows, columns, parent), m_editableRow(-1) {
    // 初始化时没有可编辑的行
}

MyEditableRowModel::MyEditableRowModel(QObject *parent):m_editableRow(-1)
{

}


// 设置某一行可编辑
void MyEditableRowModel::setEditableRow(int row) {
    if (row >= 0 && row < rowCount()) {
        m_editableRow = row;
    }
    // 如果传入的 row 越界，可以选择忽略或者打印警告（这里简单忽略）
    else{
        qDebug() << "Row index out of bounds for editable row:" << row;
    }
}

// 清除可编辑行
void MyEditableRowModel::clearEditableRow() {
    m_editableRow = -1;  // 恢复为无编辑行状态
}

// 重写 flags 方法
Qt::ItemFlags MyEditableRowModel::flags(const QModelIndex &index) const {
    // 先获取默认的 flags
    Qt::ItemFlags defaultFlags = QStandardItemModel::flags(index);

    // 如果当前行是我们设置的可编辑行，则允许编辑
    if (index.row() == m_editableRow) {
        return defaultFlags | Qt::ItemIsEditable;
    } else {
        // 其他行禁止编辑
        return defaultFlags & ~Qt::ItemIsEditable;
    }
}
