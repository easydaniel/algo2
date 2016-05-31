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
    weight = 0;
    type = t;
    left = NULL;
    right = NULL;
    parent = NULL;
  };
  // __repr__
  friend ostream& operator<< (ostream &out, const Node *n) {
    out << "(" << n->weight << "," << n->type << "," << n->symbol << ")";
    return out;
  }

};

class HuffmanTree {
public:
  // member
  Node *root;
  HuffmanTree () {
    root = new Node("NYT");
  };

  // Find specific node with symbol s
  Node* Find(unsigned char& s, Node* r) {
    if (r == NULL) {
      return NULL;
    }
    if (r->symbol == s) {
      return r;
    }
    Node* f = Find(s, r->left);
    if (f == NULL) {
      return Find(s, r->right);
    }
    return f;
  };

  // Find the NYT node
  Node* FindNYT(Node *r) {
    if (r == NULL) {
      return NULL;
    }
    if (r != root && r->type == "NYT") {
      return r;
    }
    Node *t = FindNYT(r->left);
    if (t == NULL) {
      return FindNYT(r->right);
    }
    return t;
  }

  // Get the path to the node
  string GetPath(Node *r, string p) {
    if (r->parent == NULL) {
      return p;
    } else {
      return r->parent->left == r ? GetPath(r->parent, p) + "0": GetPath(r->parent, p) + "1";
    }
  };

  void Update(Node *r) {
    // increase weight
    r->weight++;
    Node *pr = NULL;
    // Get Order list using bfs
    vector<Node*> link;
    link.push_back(root);
    int cnt = 0;
    int pos = 0;
    while (cnt < link.size()) {
      if (link[cnt] == r) {
        pos = cnt;
      }
      if (link[cnt]->right != NULL) {
        link.push_back(link[cnt]->right);
      }
      if (link[cnt]->left != NULL) {
        link.push_back(link[cnt]->left);
      }
      cnt++;
    }

    for (int i = pos; i >= 0; i--) {
      // Skip parent
      if (link[i] == r->parent) {
        continue;
      }
      // Remember the lattest small one
      if (link[i]->weight < r->weight) {
        pr = link[i];
      }
    }

    link.clear();

    // Need swap
    if (pr != NULL) {
      Node *tmp = r->parent;
      r->parent = pr->parent;
      pr->parent = tmp;

      if (r->parent->left == pr) {
        r->parent->left = r;
      } else {
        r->parent->right = r;
      }
      if (pr->parent->left == r) {
        pr->parent->left = pr;
      } else {
        pr->parent->right = pr;
      }

    }
    // Continue until r == root
    if (r->parent != NULL) {
      Update(r->parent);
    }
  }

  void Insert(unsigned char s, Node *NYT, Node *ins) {
    if (NYT == NULL) {
      // First insert
      root->type = "ROOT";
      root->weight = 1;
      root->left = new Node("NYT");
      root->left->parent = root;
      root->right = new Node("LEAF");
      root->right->parent = root;
      root->right->weight = 1;
      root->right->symbol = s;
    } else {
      if (ins == NULL) {
        // New symbol
        NYT->type = "INTERNAL";
        NYT->left = new Node("NYT");
        NYT->left->parent = NYT;
        NYT->right = new Node("LEAF");
        NYT->right->parent = NYT;
        NYT->right->symbol = s;
        ins = NYT->right;
      }
      Update(ins);
    }
  };
};

void decode(const char* src, const char* dest) {
  // file stream
  ifstream ifile(src, ios::binary | ios::ate);
  ofstream ofile(dest,ios::binary | ios::out);
  // get file size
  streamsize size = ifile.tellg();
  // initialize roll back to beginning
  ifile.seekg(0, ios::beg);
  // buffer to store
  vector<char> buffer(size);

  // Construct NYT node with some value;
  HuffmanTree HT;

  // parse symbols
  if (ifile.read(buffer.data(), size)) {
    size_t i = 0;
    while (i < size) {

      Node *CUR = HT.root;
      unsigned char c = buffer[i];
      while (CUR->type != "NYT") {
        if (c == '0') {
          CUR = CUR->left;
        } else {
          CUR = CUR->right;
        }
        if (CUR->type == "LEAF") {
          c = CUR->symbol;
          break;
        }
        i++;
        c = buffer[i];
      }

      ofile << c;
      Node *NYT = HT.FindNYT(HT.root);
      Node *ins = HT.Find(c, HT.root);
      HT.Insert(c, NYT, ins);

      i++;
    }

  } else {
    cout << "Read file error" << endl;
  }
  // Close fstream
  ofile.close();
  ifile.close();
}

void encode(const char* src, const char* dest) {
  // file stream
  ifstream ifile(src, ios::binary | ios::ate);
  ofstream ofile(dest,ios::binary | ios::out);
  // get file size
  streamsize size = ifile.tellg();
  // initialize roll back to beginning
  ifile.seekg(0, ios::beg);
  // buffer to store
  vector<char> buffer(size);

  // Construct NYT node with some value;
  HuffmanTree HT;

  // parse symbols
  if (ifile.read(buffer.data(), size)) {
    for(unsigned char c : buffer) {
      // Insert nodes
      Node *NYT = HT.FindNYT(HT.root);
      Node *ins = HT.Find(c, HT.root);
      if (NYT == NULL) {
        // first insert
        ofile << c;
      } else {
        if (ins == NULL) {
          // new symbol
          ofile << HT.GetPath(NYT, "");
          ofile << c;
        } else {
          // path to exist symbol
          ofile << HT.GetPath(ins, "");
        }
      }
      HT.Insert(c, NYT, ins);
    }


  } else {
    cout << "Read file error" << endl;
  }
  // Close fstream
  ofile.close();
  ifile.close();
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
