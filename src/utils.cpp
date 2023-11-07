
// char handleScandics(char cur, char prev) {
//     if (cur == -124 & prev == -61) { // Letter 'Ä'
//         return 65; // Letter A
//     }
//     // -1 is EOF
//     if (cur < -1 & prev >= 0) {
//         return 0;
//     }
//     return cur;
// }


// void printTree(std::shared_ptr<GNODE> node, int depth, GAST& tree) {


//     int tabs = depth;
//     string tab = "";
//     while (tabs > 0) {
//         std::cout << "\t";
//         tabs--;
//     }

//     std::clog << "type is " << node->getType() << " and id is " << node->id
//         << " and left is " << node->getLeft()
//         << " and right is " << node->getRight()
//         << " and next is "  << node->getNext()
//         << "\n";

//     std::shared_ptr<GNODE> left   = tree.getNode(node->getLeft());
//     std::shared_ptr<GNODE> right  = tree.getNode(node->getRight());
//     std::shared_ptr<GNODE> next   = tree.getNode(node->getNext());

//     if (left != nullptr) {
//         printTree(left, depth + 1, tree);
//     }

//     if (right != nullptr) {
//         printTree(right, depth + 1, tree);
//     }

//     if (next != nullptr) {
//         printTree(next, depth, tree);
//     }

//     return;
// }