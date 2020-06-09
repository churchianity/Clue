
void traverse(Node* head) {
    if (head->left) {
        traverse(head->left);

    else if (head->right) {
        traverse(head->right);

    } else {
        printToken(head->token);
    }
}
