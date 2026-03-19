#pragma once

#include <QAbstractItemModel>
#include <QPersistentModelIndex>
#include <QSet>
#include <QStringList>
#include <QVector>
#include <memory>
#include <vector>

class TreeViewServiceInterface;

class BasicTreeViewModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit BasicTreeViewModel(
        TreeViewServiceInterface* xService,
        int xBatchSize = 40,
        QObject* xParent = nullptr);
    ~BasicTreeViewModel() override;

    QModelIndex index(int xRow, int xColumn, const QModelIndex& xParent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& xChild) const override;
    int rowCount(const QModelIndex& xParent = QModelIndex()) const override;
    int columnCount(const QModelIndex& xParent = QModelIndex()) const override;
    QVariant data(const QModelIndex& xIndex, int xRole = Qt::DisplayRole) const override;
    QVariant headerData(int xSection, Qt::Orientation xOrientation, int xRole = Qt::DisplayRole) const override;
    bool hasChildren(const QModelIndex& xParent = QModelIndex()) const override;
    bool canFetchMore(const QModelIndex& xParent) const override;
    void fetchMore(const QModelIndex& xParent) override;

    void mResetModel();

    void mSetFilterPattern(const QString& xPattern);
    void mSetSortColumn(int xColumnIndex, Qt::SortOrder xOrder);
    void mClearFiltersAndSort();

private:
    struct TreeNode
    {
        int dId = -1;
        QVector<QVariant> dColumns;
        int dTotalChildren = 0;
        TreeNode* dParent = nullptr;
        std::vector<std::unique_ptr<TreeNode>> dChildren;

        int mRow() const;
    };

    TreeNode* mNodeFromIndex(const QModelIndex& xIndex) const;
    void mLoadChildren(TreeNode* xParentNode, const QModelIndex& xParentIndex, int xLimit);
    void mInitializeRoot();
    void mRequestRootItemCount();

    TreeViewServiceInterface* dService;
    QStringList dHeaders;
    int dBatchSize;
    std::unique_ptr<TreeNode> dRoot;
    QSet<TreeNode*> dLoadingNodes;
    int dGeneration;
};
