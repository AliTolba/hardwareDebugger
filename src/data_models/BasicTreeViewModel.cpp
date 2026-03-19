#include "BasicTreeViewModel.h"

#include "ViewDataServices.h"

BasicTreeViewModel::BasicTreeViewModel(
    TreeViewServiceInterface* xService,
    int xBatchSize,
    QObject* xParent)
    : QAbstractItemModel(xParent)
    , dService(xService)
    , dHeaders(xService != nullptr ? xService->mHeaders() : QStringList())
    , dBatchSize(xBatchSize)
{
    mInitializeRoot();
    fetchMore(QModelIndex());
}

BasicTreeViewModel::~BasicTreeViewModel() = default;

QModelIndex BasicTreeViewModel::index(int xRow, int xColumn, const QModelIndex& xParent) const
{
    if (!hasIndex(xRow, xColumn, xParent)) {
        return QModelIndex();
    }

    TreeNode* lParentNode = mNodeFromIndex(xParent);
    if (lParentNode == nullptr || xRow < 0 || xRow >= lParentNode->dChildren.size()) {
        return QModelIndex();
    }

    return createIndex(xRow, xColumn, lParentNode->dChildren.at(xRow).get());
}

QModelIndex BasicTreeViewModel::parent(const QModelIndex& xChild) const
{
    if (!xChild.isValid()) {
        return QModelIndex();
    }

    TreeNode* lChildNode = mNodeFromIndex(xChild);
    TreeNode* lParentNode = lChildNode != nullptr ? lChildNode->dParent : nullptr;
    if (lParentNode == nullptr || lParentNode == dRoot.get()) {
        return QModelIndex();
    }

    return createIndex(lParentNode->mRow(), 0, lParentNode);
}

int BasicTreeViewModel::rowCount(const QModelIndex& xParent) const
{
    if (xParent.column() > 0) {
        return 0;
    }

    TreeNode* lParentNode = mNodeFromIndex(xParent);
    return lParentNode != nullptr ? lParentNode->dChildren.size() : 0;
}

int BasicTreeViewModel::columnCount(const QModelIndex& xParent) const
{
    Q_UNUSED(xParent)
    return dHeaders.size();
}

QVariant BasicTreeViewModel::data(const QModelIndex& xIndex, int xRole) const
{
    if (!xIndex.isValid() || xRole != Qt::DisplayRole) {
        return QVariant();
    }

    TreeNode* lNode = mNodeFromIndex(xIndex);
    if (lNode == nullptr || xIndex.column() < 0 || xIndex.column() >= lNode->dColumns.size()) {
        return QVariant();
    }

    return lNode->dColumns.at(xIndex.column());
}

QVariant BasicTreeViewModel::headerData(int xSection, Qt::Orientation xOrientation, int xRole) const
{
    if (xOrientation != Qt::Horizontal || xRole != Qt::DisplayRole) {
        return QVariant();
    }

    return xSection >= 0 && xSection < dHeaders.size() ? QVariant(dHeaders.at(xSection)) : QVariant();
}

bool BasicTreeViewModel::hasChildren(const QModelIndex& xParent) const
{
    TreeNode* lNode = mNodeFromIndex(xParent);
    return lNode != nullptr && lNode->dTotalChildren > 0;
}

bool BasicTreeViewModel::canFetchMore(const QModelIndex& xParent) const
{
    TreeNode* lNode = mNodeFromIndex(xParent);
    return lNode != nullptr && lNode->dChildren.size() < lNode->dTotalChildren;
}

void BasicTreeViewModel::fetchMore(const QModelIndex& xParent)
{
    TreeNode* lNode = mNodeFromIndex(xParent);
    if (lNode == nullptr) {
        return;
    }

    const int lRemaining = lNode->dTotalChildren - lNode->dChildren.size();
    if (lRemaining <= 0) {
        return;
    }

    mLoadChildren(lNode, xParent, qMin(dBatchSize, lRemaining));
}

void BasicTreeViewModel::mResetModel()
{
    beginResetModel();
    dHeaders = dService != nullptr ? dService->mHeaders() : QStringList();
    mInitializeRoot();
    endResetModel();
    fetchMore(QModelIndex());
}

int BasicTreeViewModel::TreeNode::mRow() const
{
    if (dParent == nullptr) {
        return 0;
    }

    for (int lI = 0; lI < dParent->dChildren.size(); ++lI) {
        if (dParent->dChildren.at(lI).get() == this) {
            return lI;
        }
    }

    return 0;
}

BasicTreeViewModel::TreeNode* BasicTreeViewModel::mNodeFromIndex(const QModelIndex& xIndex) const
{
    if (!xIndex.isValid()) {
        return dRoot.get();
    }

    return static_cast<TreeNode*>(xIndex.internalPointer());
}

void BasicTreeViewModel::mLoadChildren(TreeNode* xParentNode, const QModelIndex& xParentIndex, int xLimit)
{
    if (dService == nullptr || xParentNode == nullptr || xLimit <= 0) {
        return;
    }

    const QVariant lParentId = xParentNode == dRoot.get() ? QVariant() : QVariant(xParentNode->dId);
    const QVector<TreeItemData> lItems = dService->mFetchChildren(lParentId, xParentNode->dChildren.size(), xLimit);
    if (lItems.isEmpty()) {
        return;
    }

    const int lFirst = xParentNode->dChildren.size();
    const int lLast = lFirst + lItems.size() - 1;
    beginInsertRows(xParentIndex, lFirst, lLast);
    for (const TreeItemData& lItem : lItems) {
        std::unique_ptr<TreeNode> lChild = std::make_unique<TreeNode>();
        lChild->dId = lItem.dId;
        lChild->dColumns = lItem.dColumns;
        lChild->dTotalChildren = lItem.dChildCount;
        lChild->dParent = xParentNode;
        xParentNode->dChildren.push_back(std::move(lChild));
    }
    endInsertRows();
}

void BasicTreeViewModel::mInitializeRoot()
{
    dRoot = std::make_unique<TreeNode>();
    dRoot->dTotalChildren = dService != nullptr ? dService->mItemCount(QVariant()) : 0;
}
