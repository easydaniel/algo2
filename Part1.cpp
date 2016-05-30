#include <iostream>
#include <vector>
#include <string>
#include <fstream>
using namespace std;

class Node {
public:
  // member
  unsigned char symbol;
  int weight;
  string type;
  Node *left, *right, *parent;
  Node (string t) {
    symbol = 'X';
    weight = (t != "NYT");
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

  HuffmanTree () {
    root = new Node("ROOT");
    NYT = root;
  };

  Node* Find(unsigned char& s, Node* r) {
    if (r == NULL) {
      return NULL;
    }
    if (r->type == "LEAF" && r->symbol == s) {
      return r;
    }
    Node* f = Find(s, r->left);
    if (f == NULL) {
      return Find(s, r->right);
    }
    return f;
  };

  void InOrderTranverse(Node* r) {
    if (r == NULL) {
      return;
    }
    InOrderTranverse(r->left);
    cout << r->symbol << ' ' << r->weight << ' ' << r->type << endl;
    InOrderTranverse(r->right);
  }

  void Insert(unsigned char s) {
    if (NYT == root) {
      NYT = new Node("NYT");
      NYT->parent = root;
      root->left = NYT;
      root->left->parent = root;
      root->right = new Node("LEAF");
      root->right->symbol = s;
      root->right->parent = root;
    } else {
      Node* r = Find(s, root);
      if (r == NULL) {
        // New symbol
        Node* now = NYT;
        now->type = "INTERNAL";
        now->left = new Node("NYT");
        now->right = new Node("LEAF");
        now->right->symbol = s;
        now->left->parent = now;
        now->right->parent = now;
        NYT = now->left;
        while (now->parent != NULL) {
          now->weight++;
          now = now->parent;
        }
        now->weight++;
      } else {
        // Exist - add weight to ancestors
        while(r->parent != NULL) {
          r->weight++;
          r = r->parent;
        }
        // r == root
        r->weight++;
      }
      // Maintain tree
      cout << "----- Insert " << s << " -----" << endl;
      this->InOrderTranverse(root);
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
  HuffmanTree HT;

  // parse symbols
  if (file.read(buffer.data(), size)) {
    for(unsigned char c : buffer) {
      HT.Insert(c);
    }

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
