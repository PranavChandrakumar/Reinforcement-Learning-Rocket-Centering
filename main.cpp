#include <math.h>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <random>
#include <stack>
#include <vector>
#include <queue>

#include "RocketCentering.h"

using namespace std;

// return a double unifomrly sampled in (0,1)
double randDouble(mt19937& rng) {
  return std::uniform_real_distribution<>{0, 1}(rng);
}
// return uniformly sampled 0 or 1
bool randChoice(mt19937& rng) {
  return std::uniform_int_distribution<>{0, 1}(rng);
}
// return a random integer uniformly sampled in (min, max)
int randInt(mt19937& rng, const int& min, const int& max) {
  return std::uniform_int_distribution<>{min, max}(rng);
}

// return true if op is a suported operation, otherwise return false
bool isOp(string op) {
  if (op == "+")
    return true;
  else if (op == "-")
    return true;
  else if (op == "*")
    return true;
  else if (op == "/")
    return true;
  else if (op == ">")
    return true;
  else if (op == "abs")
    return true;
  else
    return false;
}

int arity(string op) {
  if (op == "abs")
    return 1;
  else
    return 2;
}

typedef string Elem;

class LinkedBinaryTree {
 public:
  struct Node {
    Elem elt;
    string name;
    Node* par;
    Node* left;
    Node* right;
    Node() : elt(), par(NULL), name(""), left(NULL), right(NULL) {}
    int depth() {
      if (par == NULL) return 0;
      return par->depth() + 1;
    }
  };

  class Position {
   private:
    Node* v;

   public:
    Position(Node* _v = NULL) : v(_v) {}
    Elem& operator*() { return v->elt; }
    Position left() const { return Position(v->left); }
    void setLeft(Node* n) { v->left = n; }
    Position right() const { return Position(v->right); }
    void setRight(Node* n) { v->right = n; }
    Position parent() const  // get parent
    {
      return Position(v->par);
    }
    bool isRoot() const  // root of the tree?
    {
      return v->par == NULL;
    }
    bool isExternal() const  // an external node?
    {
      return v->left == NULL && v->right == NULL;
    }
    friend class LinkedBinaryTree;  // give tree access
  };
  typedef vector<Position> PositionList;

 public:
  LinkedBinaryTree() : _root(NULL), score(0), steps(0), generation(0) {}

  // copy constructor
  LinkedBinaryTree(const LinkedBinaryTree& t) {
    _root = copyPreOrder(t.root());
    score = t.getScore();
    steps = t.getSteps();
    generation = t.getGeneration();
  }

  // copy assignment operator
  LinkedBinaryTree& operator=(const LinkedBinaryTree& t) {
    if (this != &t) {
      // if tree already contains data, delete it
      if (_root != NULL) {
        PositionList pl = positions();
        for (auto& p : pl) delete p.v;
      }
      _root = copyPreOrder(t.root());
      score = t.getScore();
      steps = t.getSteps();
      generation = t.getGeneration();
    }
    return *this;
  }

  // destructor
  ~LinkedBinaryTree() {
    if (_root != NULL) {
      PositionList pl = positions();
      for (auto& p : pl) delete p.v;
    }
  }

  int size() const { return size(_root); }
  int size(Node* root) const;
  int depth() const;
  bool empty() const { return size() == 0; };
  Node* root() const { return _root; }
  PositionList positions() const;
  void addRoot() { _root = new Node; }
  void addRoot(Elem e) {
    _root = new Node;
    _root->elt = e;
  }
  void nameRoot(string name) { _root->name = name; }
  void addLeftChild(const Position& p, const Node* n);
  void addLeftChild(const Position& p);
  void addRightChild(const Position& p, const Node* n);
  void addRightChild(const Position& p);
  void printExpression() { printExpression(_root); }
  void printExpression(Node* v);
  double evaluateExpression(double a, double b) {
    return evaluateExpression(Position(_root), a, b);
  };
  double evaluateExpression(const Position& p, double a, double b);
  long getGeneration() const { return generation; }
  void setGeneration(int g) { generation = g; }
  double getScore() const { return score; }
  void setScore(double s) { score = s; }
  double getSteps() const { return steps; }
  void setSteps(double s) { steps = s; }
  void randomExpressionTree(Node* p, const int& maxDepth, mt19937& rng);
  void randomExpressionTree(const int& maxDepth, mt19937& rng) {
    randomExpressionTree(_root, maxDepth, rng);
  }
  void Crossover(mt19937 &rng, LinkedBinaryTree &P1, LinkedBinaryTree &P2); //Declaration of crossover function
  void deleteSubtreeMutator(mt19937 &rng);
  void addSubtreeMutator(mt19937& rng, const int maxDepth);
  bool LexLessThan(const LinkedBinaryTree &A, const LinkedBinaryTree &B); //Decleration of LexLessThan function

protected:                                         // local utilities
  void preorder(Node* v, PositionList& pl) const;  // preorder utility
  Node* copyPreOrder(const Node* root);
  double score;     // mean reward over 20 episodes
  double steps;     // mean steps-per-episode over 20 episodes
  long generation;  // which generation was tree "born"
 private:
  Node* _root;  // pointer to the root
};

// add the tree rooted at node child as this tree's left child
void LinkedBinaryTree::addLeftChild(const Position& p, const Node* child) {
  Node* v = p.v;
  v->left = copyPreOrder(child);  // deep copy child
  v->left->par = v;
}

// add the tree rooted at node child as this tree's right child
void LinkedBinaryTree::addRightChild(const Position& p, const Node* child) {
  Node* v = p.v;
  v->right = copyPreOrder(child);  // deep copy child
  v->right->par = v;
}

void LinkedBinaryTree::addLeftChild(const Position& p) {
  Node* v = p.v;
  v->left = new Node;
  v->left->par = v;
}

void LinkedBinaryTree::addRightChild(const Position& p) {
  Node* v = p.v;
  v->right = new Node;
  v->right->par = v;
}

// return a list of all nodes
LinkedBinaryTree::PositionList LinkedBinaryTree::positions() const {
  PositionList pl;
  preorder(_root, pl);
  return PositionList(pl);
}

void LinkedBinaryTree::preorder(Node* v, PositionList& pl) const {
  pl.push_back(Position(v));
  if (v->left != NULL) preorder(v->left, pl);
  if (v->right != NULL) preorder(v->right, pl);
}

int LinkedBinaryTree::size(Node* v) const {
  int lsize = 0;
  int rsize = 0;
  if (v->left != NULL) lsize = size(v->left);
  if (v->right != NULL) rsize = size(v->right);
  return 1 + lsize + rsize;
}

int LinkedBinaryTree::depth() const {
  PositionList pl = positions();
  int depth = 0;
  for (auto& p : pl) depth = std::max(depth, p.v->depth());
  return depth;
}

LinkedBinaryTree::Node* LinkedBinaryTree::copyPreOrder(const Node* root) {
  if (root == NULL) return NULL;
  Node* nn = new Node;
  nn->elt = root->elt;
  nn->left = copyPreOrder(root->left);
  if (nn->left != NULL) nn->left->par = nn;
  nn->right = copyPreOrder(root->right);
  if (nn->right != NULL) nn->right->par = nn;
  return nn;
}

void LinkedBinaryTree::printExpression(Node *v)
{
  // replace print statement with your code
  // Recursive function, calls itself to print left and right subtrees
  if (Position(v).isExternal())//Condition to check if the current node is external
  {
    std::cout << v->elt;
  }
  else
  {
    if (v->elt == "abs") //This operator is special, and requires only the left child to be printed 
    {
      std::cout << v->elt << "(";
      printExpression(v->left);
    }
    else
    {
      std::cout << "(";
      printExpression(v->left);
      std::cout << v->elt;
      printExpression(v->right);
    }
    std::cout << ")";
  }
}

double evalOp(string op, double x, double y = 0) {
  double result;
  if (op == "+")
    result = x + y;
  else if (op == "-")
    result = x - y;
  else if (op == "*")
    result = x * y;
  else if (op == "/") {
    result = x / y;
  } else if (op == ">") {
    result = x > y ? 1 : -1;
  } else if (op == "abs") {
    result = abs(x);
  } else
    result = 0;
  return isnan(result) || !isfinite(result) ? 0 : result;
}

double LinkedBinaryTree::evaluateExpression(const Position& p, double a,
                                            double b) {
  if (!p.isExternal()) {
    auto x = evaluateExpression(p.left(), a, b);
    if (arity(p.v->elt) > 1) {
      auto y = evaluateExpression(p.right(), a, b);
      return evalOp(p.v->elt, x, y);
    } else {
      return evalOp(p.v->elt, x);
    }
  } else {
    if (p.v->elt == "a")
      return a;
    else if (p.v->elt == "b")
      return b;
    else
      return stod(p.v->elt);
  }
}

void LinkedBinaryTree::Crossover(mt19937 &rng, LinkedBinaryTree &P1, LinkedBinaryTree &P2)
{
  //Find a subtree in P1 and P2, and switch them accordingly
  Node *P1cur = P1._root;
  Node *P2cur = P2._root;

  if (P1cur == P2cur || P1cur == nullptr || P2cur == nullptr){
    return; //Make sure that the two parents are not the same, and that the two trees are valid
  }

  Position pos1; //The following two loops are similar to the mutator tree traversal. It's simply a method to find two random nodes within a tree.
  Position pos2;
  int Decision;
  while (P1cur != NULL)
  {
    Decision = randInt(rng, 1, 3);
    if (Decision == 1)
    {
      if (randChoice(rng) && P1cur->left != nullptr)
      {
        pos1 = P1cur->left;
      }
      else if (P1cur->right != nullptr)
      {
        pos1 = P1cur->right;
      }
      break;
    }
    else if (Decision == 2)
    {
      P1cur = P1cur->left;
    }
    else if (Decision == 3)
    {
      P1cur = P1cur->right;
    }
  }
  Decision = 0;
  while (P2cur != NULL)
  {
    Decision = randInt(rng, 1, 3);
    if (Decision == 1)
    {
      if (randChoice(rng) && P2cur->left != nullptr)
      {
        pos2 = P2cur->left;
      }
      else if (P2cur->right != nullptr)
      {
        pos2 = P2cur->right;
      }
      break;
    }
    else if (Decision == 2)
    {
      P2cur = P2cur->left;
    }
    else if (Decision == 3)
    {
      P2cur = P2cur->right;
    }
  }

  if(pos1.v != NULL && pos2.v != NULL && pos1.v->depth() == pos2.v->depth()){//Only crossover if the nodes are not null, AND the depths are equal (to ensure max depth is not exceeded in either parent)
    Position parent1 = pos1.parent();
    Position parent2 = pos2.parent();

    Node* temp = copyPreOrder(pos1.v);//Copy a subtree into a temporary node as the subtree root, then use this for the swap.

    if (parent1.left().v == pos1.v)
    {
      parent1.setLeft(pos2.v);
    }
    else
    {
      parent1.setRight(pos2.v);
    }

    if (parent2.left().v == pos2.v)
    {
      parent2.setLeft(temp);
    }
    else
    {
      parent2.setRight(temp);
    }
    pos2.parent() = parent1.v;
    pos1.parent() = parent2.v;
  }
}

void LinkedBinaryTree::deleteSubtreeMutator(mt19937& rng) {
  // your code here...
  Node* curNode = _root; //Node to be iterated through the tree
  Node *STRoot = nullptr; //Node which will be selected as the root of the subtree 
  Position pos; //Position class member to save position information of the subtree root node
  int Decision; //Decision variable to store the decision for the current node (1 for one of the current node's children will be the STRoot, 2 for left child progression, 3 for right child progression)
  while (curNode != NULL) {//Only want to consider a STRoot that is not NULL
    Decision = randInt(rng,1,3);
    if (Decision == 1){
      if (randChoice(rng) && curNode->left != nullptr){ //Checking that the child node is not null to ensure the current node is not external
        STRoot = curNode->left;
        pos = curNode->left;
      }
      else if (curNode->right != nullptr){
        STRoot = curNode->right;
        pos = curNode->right;
      }
      break; //No more tree traversal is required, the STRoot has been selected
    }
    else if (Decision == 2){
      curNode = curNode->left;
    }
    else if (Decision == 3){
      curNode = curNode->right;
    }
  }

  if(STRoot != nullptr){//Deletion will only occur if the selected node is not NULL (slightly redundant to have this condition, but good practice for defensive programming)
    PositionList subtree;
    preorder(pos.v, subtree);//Assigning all positions to the selected node past its current depth, originating from the node itself
    Position parent = pos.parent();

    if(parent.left().v == pos.v){//Condition to check if the STRoot is the left or right child of its parent
      parent.setLeft(new Node); //Creating new node to store an operand (in place of the STRoot node)
        if(randChoice(rng)){
          parent.left().v->elt = "b";
        }
        else{
          parent.left().v->elt = "a";
        }
    }
    else{//STRoot is the right child of its parent
      parent.setRight(new Node);
      if (randChoice(rng)) 
      {
        parent.right().v->elt = "a";
      }
      else
      {
        parent.right().v->elt = "b";
      }
    }

    for (auto& p : subtree){//Deletion of the subtree
      delete p.v;
    }
  } 
}

LinkedBinaryTree createRandExpressionTree(int max_depth, mt19937 &rng); //Forward declaration of createRandExpressionTree, need it for implementation in addMutator

void LinkedBinaryTree::addSubtreeMutator(mt19937 &rng, const int maxDepth)
{
  // your code here...
  //Get the max depth of the current tree
  
  Node *curNode = _root; //Similar logic as the deleteSubtreeMutator
  Node *STRoot = nullptr;
  Position pos;
  int Decision;

  while (curNode != NULL)
  {
    Decision = randInt(rng, 1, 3);
    if (Decision == 1)
    {
      if (curNode->left == nullptr && curNode->right == nullptr){ //We want to ensure that the posiion the subtree will be added in is an external node
        pos = curNode;
        STRoot = curNode;
      }
      break;//STRoot has been selected
    }
    else if (Decision == 2 && curNode->left != nullptr){
      curNode = curNode->left;
    }
    else if (Decision == 3 && curNode->right != nullptr){
      curNode = curNode->right;
    }
  }

  if (STRoot != nullptr)
  {
    LinkedBinaryTree SubTree = createRandExpressionTree((maxDepth - pos.v->depth()), rng);//Creating a subtree which will not cause the main tree to exceed the max depth
    if(!pos.isRoot()){ //Check to make sure that the STRoot is itself not the root of the host tree
      Position Parent = pos.parent();
      if(Parent.left().v == pos.v){//Condition to check if the STRoot is the left or right child of parent node
        addLeftChild(Parent,SubTree.root());
      }
      else{
        addRightChild(Parent,SubTree.root());
      }
    }
    else{
      pos = SubTree.root(); //In the case of the STRoot being the root of the parent tree, we make the STRoot the new root of the host tree
    }
  }
  
}

bool operator<(const LinkedBinaryTree& x, const LinkedBinaryTree& y) {
  return x.getScore() < y.getScore();
}

LinkedBinaryTree createExpressionTree(string postfix) {
  stack<LinkedBinaryTree> tree_stack;
  stringstream ss(postfix);
  // Split each line into words
  string token;
  while (getline(ss, token, ' ')) {
    LinkedBinaryTree t;
    if (!isOp(token)) {
      t.addRoot(token);
      tree_stack.push(t);
    } else {
      t.addRoot(token);
      if (arity(token) > 1) {
        LinkedBinaryTree r = tree_stack.top();
        tree_stack.pop();
        t.addRightChild(t.root(), r.root());
      }
      LinkedBinaryTree l = tree_stack.top();
      tree_stack.pop();
      t.addLeftChild(t.root(), l.root());
      tree_stack.push(t);
    }
  }
  return tree_stack.top();
}

LinkedBinaryTree createRandExpressionTree(int max_depth, mt19937& rng) {
  // modify this function to create and return a random expression tree
  
  string tree = ""; //The string which will contain the random expression tree, to be passed to createExpressionTree
  LinkedBinaryTree t; //Creating a new tree
  int Ubound = randInt(rng, 0, max_depth); //Determining the upper bound of the random tree's depth
  if (Ubound == 0){ //If the new tree has a depth of 0, only one of the operands needs to be appended to the tree string
    if (randChoice(rng)){
      tree.append("a");
    }
    else{
      tree.append("b");
    }
    t = createExpressionTree(tree); //Tree is created, as no more nodes need to be added
    return t;
  }
  
  int CurrentPos = 0; //Depth counter
  int Decision = 0; //1 for a or b, 3 for abs, 2 for operator not abs, initialized to 0 for first choice
  int OpDecision = 0; //Variable for the decision of operator choice (not abs)
  
  while (CurrentPos < Ubound){ 
    if (CurrentPos == 0 && Decision == 0 && Ubound != 0){// First term of a non-zero depth tree
      if (randChoice(rng)){
        tree.append("a");
      }
      else{
        tree.append("b");
      }
      if(randChoice(rng)){ //After first operand of the tree, either abs or another operand can be added to the tree
        Decision = 1;
      }
      else{
        Decision = 3;
      }
    }
    else{
      if (Decision == 1){//Adding an operand to the tree. The depth counter (CurrentPos) does not need to be incrememnted, as this node will be at the same depth as the node prior to it
        if (randChoice(rng)){
          tree.append(" a");
        }
        else{
          tree.append(" b");
        }
        Decision = 2; //Two operands have been added sequentially to the string, so a non-abs operator needs to be next
      }
      else if (Decision == 2){//A non-abs operator needs to be next
        OpDecision = randInt(rng,1,5); //Random number generator to eselct one of the operators
        if (OpDecision == 1){
          tree.append(" +");
        }
        else if (OpDecision == 2){
          tree.append(" -");
        }
        else if (OpDecision == 3){
          tree.append(" *");
        }
        else if (OpDecision == 4){
          tree.append(" /");
        }
        else{
          tree.append(" >");
        }
        if (randChoice(rng)){
          Decision = 1;
        }
        else{
          Decision = 3;
        }
        CurrentPos++; //Increment the depth counter, as addition of an operator will be at a higher depth
      }
      else{
        tree.append(" abs");
        Decision = 1; //After adding an abs operator, the next term must be an operand
        CurrentPos++; //Increment depth counter
      }
    }
  }
  //cout << tree << endl;
  t = createExpressionTree(tree);
  return t; //Return the expression tree of a non-zero depth
}

// evaluate tree t in the cart centering task
void evaluate(mt19937& rng, LinkedBinaryTree& t, const int& num_episode,
              bool animate) {
  cartCentering env;
  double mean_score = 0.0;
  double mean_steps = 0.0;
  for (int i = 0; i < num_episode; i++) {
    double episode_score = 0.0;
    int episode_steps = 0;
    env.reset(rng);
    while (!env.terminal()) {
      int action = t.evaluateExpression(env.getCartXPos(), env.getCartXVel());
      episode_score += env.update(action, animate);
      episode_steps++;
    }
    mean_score += episode_score;
    mean_steps += episode_steps;
  }
  t.setScore(mean_score / num_episode);
  t.setSteps(mean_steps / num_episode);
}

bool LexLessThan(const LinkedBinaryTree &A, const LinkedBinaryTree &B) //Two different trees need to be passed in as arguments, in order to compare the two
{
  //Do a comparison between two 
  double score_diff = A.getScore() - B.getScore(); //Determining the numerical difference of score
  if (abs(score_diff) < 0.01)
  {
    return A.size() > B.size(); // Size comparison if score difference is minimal
  }
  else
  {
    return score_diff < 0; // Score comparison if score difference is too significant
  }
}

int main() {
  // Experiment parameters
  mt19937 rng(42);
  const int NUM_TREE = 50;
  const int MAX_DEPTH_INITIAL = 1;
  const int MAX_DEPTH = 20;
  const int NUM_EPISODE = 20;
  const int MAX_GENERATIONS = 100;

  // Create an initial "population" of expression trees
  vector<LinkedBinaryTree> trees;
  for (int i = 0; i < NUM_TREE; i++) {
    LinkedBinaryTree t = createRandExpressionTree(MAX_DEPTH_INITIAL, rng);
    trees.push_back(t);
  }

  // Genetic Algorithm loop
  LinkedBinaryTree best_tree;
  std::cout << "generation,fitness,steps,size,depth" << std::endl;
  for (int g = 1; g <= MAX_GENERATIONS; g++) {

    // Fitness evaluation
    for (auto& t : trees) {
      if (t.getGeneration() < g - 1) continue;  // skip if not new
      evaluate(rng, t, NUM_EPISODE, false);
    }

    // sort trees using overloaded "<" op (worst->best)
    std::sort(trees.begin(), trees.end());

    // // sort trees using comparaor class (worst->best)
    std::sort(trees.begin(), trees.end(), LexLessThan);//LexLessThan is a comparator function, so arguments are implicitly passed in during sort function processing

    // erase worst 50% of trees (first half of vector)
    trees.erase(trees.begin(), trees.begin() + NUM_TREE / 2);

    // Print stats for best tree
    best_tree = trees[trees.size() - 1];
    std::cout << g << ",";
    std::cout << best_tree.getScore() << ",";
    std::cout << best_tree.getSteps() << ",";
    std::cout << best_tree.size() << ",";
    std::cout << best_tree.depth() << std::endl;

    //Crossover
    //trees[randInt(rng, 0, NUM_TREE)].Crossover(rng,trees[randInt(rng, 0, NUM_TREE)], trees[randInt(rng, 1, NUM_TREE)]); //Calling the crossover function with two different trees to undergo switch

    // Selection and mutation
    while (trees.size() < NUM_TREE) {
      // Selected random "parent" tree from survivors
      LinkedBinaryTree parent = trees[randInt(rng, 0, (NUM_TREE / 2) - 1)];
      
      // Create child tree with copy constructor
      LinkedBinaryTree child(parent);
      child.setGeneration(g);
      
      // Mutation
      // Delete a randomly selected part of the child's tree
      child.deleteSubtreeMutator(rng);
      // Add a random subtree to the child
      child.addSubtreeMutator(rng, MAX_DEPTH);
      
      trees.push_back(child);
    }
  }



  // Evaluate best tree with animation
  const int num_episode = 3;
  evaluate(rng, best_tree, num_episode, true);

  // Print best tree info
  
  std::cout << std::endl << "Best tree:" << std::endl;
  best_tree.printExpression();
  std::cout << endl;
  std::cout << "Generation: " << best_tree.getGeneration() << endl;
  std::cout << "Size: " << best_tree.size() << std::endl;
  std::cout << "Depth: " << best_tree.depth() << std::endl;
  std::cout << "Fitness: " << best_tree.getScore() << std::endl << std::endl;
}