#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <queue>
using namespace std;


class Node {
public:
  // member
  unsigned char symbol;
  int weight;
  string type;
  Node *left, *right, *parent;

  Node (unsigned char s, string t) {
    symbol = s;
    weight = (t == "NYT" ? 0 : 1);
    type = t;
    left = NULL;
    right = NULL;
    parent = NULL;
  };

};



class HuffmanTree {
public:
  // member
  Node *root, *NYT;

  HuffmanTree (unsigned char s, string t) {
    root = new Node(s, t);
    NYT = root;
  };

  Node* Find(unsigned char& s, Node* r) {
    if (r->symbol == s) {
      return r;
    } else {
      if (r->left != NULL and r->left->type != "NYT") {
        return Find(s, r->left);
      } else if (r->right != NULL and r->right->type != "NYT") {
        return Find(s, r->right);
      } else {
        return NULL;
      }
    }
  };

  void Insert(unsigned char s, string t) {
    if (NYT == root) {
      NYT = new Node('@', "NYT");
      NYT->parent = root;
      root->left = NYT;
      root->right = new Node(s, w, t);
      root->right->parent = root;
    } else {
      Node* s = Find(s, root);
      if (s == NULL) {
        // New symbol
        Node* p = NYT->parent;
        NYT->left = new Node('@', "NYT");
      } else {
        // Exist - add weight to ancestors
        while(s->parent != NULL) {
          s->weight++;
          s = s->parent;
        }
        // s == root
        s->weight++;
      }
      // Maintain tree
    }
  };

};

void decode(const char* src, const char* dest) {
  cout << "Decode" << endl;
}

void encode(const char* src, const char* dest) {
  // file stream
  ifstream file(src, ios::binary | ios::ate);
  // get file size
  streamsize size = file.tellg();
  // initialize roll back to beginning
  file.seekg(0, ios::beg);
  // buffer to store
  vector<char> buffer(size);

  // Construct NYT node with some value;
  HuffmanTree HT('@', "NYT");

  // parse symbols
  if (file.read(buffer.data(), size)) {

    Node* NYT = HT.GetNYT(HT.root);
    cout << NYT->type << " " << NYT->weight << endl;

    for(unsigned char c : buffer) {
      Node* cur = HT.Find(c, HT.root);
      if (cur == NULL) {
        // new symbol - construct one
        HT.Insert(c, "LEAF");
      } else {
        // old symbol - add weight
        cout << cur->symbol << " " << cur->weight << " " << cur->type << " exist" << endl;
      }
    }

    cout << NYT->type << " " << NYT->weight << endl;

  } else {
    cout << "Read file error" << endl;
  }
}

int main(int argc, char const *argv[]) {
  if (argc < 4) {
    cout << "Not enough arguments: [E/D] Source Destination" << endl;
    return 1;
  }
  if (argv[1][0] == 'E') {
    encode(argv[2], argv[3]);
  } else if (argv[1][0] == 'D') {
    decode(argv[2], argv[3]);
  } else {
    cout << "Unknown command: " << argv[1] << endl;
  }
  return 0;
}
