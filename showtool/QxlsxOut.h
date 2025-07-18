#ifndef QXLSXOUT_H
#define QXLSXOUT_H
#include <QFileDialog>
#include <QMessageBox>
#include "QXlsx/header/xlsxdocument.h"
#include <QVector>
#include <QString>

inline bool exportDataToExcel(const QStringList& headers, const QVector<QVector<QVariant>>& data) {
    // 弹出文件保存对话框
    QString filePath = QFileDialog::getSaveFileName(
        nullptr,
        "导出Excel文件",
        QDir::homePath(),
        "Excel Files (*.xlsx);;All Files (*)"
        );

    if (filePath.isEmpty()) {
        return false; // 用户取消了操作
    }

    // 确保文件扩展名为.xlsx
    if (!filePath.endsWith(".xlsx", Qt::CaseInsensitive)) {
        filePath += ".xlsx";
    }

    // 创建Excel文档
    QXlsx::Document xlsx;

    // 设置表头样式
    QXlsx::Format headerFormat;
    headerFormat.setFontBold(true); // 粗体
    headerFormat.setFontSize(12); // 字号12
    headerFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter); // 水平居中
    headerFormat.setVerticalAlignment(QXlsx::Format::AlignVCenter); // 垂直居中
    headerFormat.setFillPattern(QXlsx::Format::PatternSolid); // 实心填充
    headerFormat.setPatternForegroundColor(QColor(230, 230, 230)); // 浅灰色背景
    headerFormat.setPatternBackgroundColor(QColor(255, 255, 255)); // 白色背景
    headerFormat.setFontColor(QColor(0, 0, 0)); // 黑色字体
    headerFormat.setBorderStyle(QXlsx::Format::BorderThin); // 细边框
    headerFormat.setBorderColor(QColor(0, 0, 0)); // 黑色边框


    // 写入表头
    for (int col = 0; col < headers.size(); ++col) {
        xlsx.write(1, col + 1, headers[col], headerFormat); // 应用表头样式
    }

    // 写入数据
    for (int row = 0; row < data.size(); ++row) {
        const auto& rowData = data[row];
        for (int col = 0; col < rowData.size(); ++col) {
            xlsx.write(row + 2, col + 1, rowData[col]);
        }
    }

    // 设置列宽自适应内容
    for (int col = 1; col <= headers.size(); ++col) {
        xlsx.setColumnWidth(col, 15);
    }

    // 保存文件
    if (xlsx.saveAs(filePath)) {
        return true;
    } else {
        QMessageBox::critical(nullptr, "失败", "Excel导出失败！");
        return false;
    }
}

#endif // QXLSXOUT_H
