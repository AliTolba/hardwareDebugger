#pragma once

#include <QWidget>

class QLabel;
class QModelIndex;
class QTableWidget;

class SelectionDetailsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SelectionDetailsWidget(QWidget* xParent = nullptr);

    void mDisplayIndex(const QModelIndex& xIndex);
    void mClear();

private:
    void mCreateGui();

    QLabel* dTitleLabel;
    QTableWidget* dDetailsTable;
};
