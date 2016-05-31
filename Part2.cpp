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

  // Find the node with symbol s
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
  string GetPath(Node *r, string &p) {
    if (r->parent == NULL) {
      return p;
    } else {
      string t = GetPath(r->parent, p);
      return r->parent->left == r ? t + "0": t + "1";
    }
  };

  void Update(Node *r) {

    r->weight++;
    Node *pr = NULL;

    // Get Order list
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

// convert char to binary repr
string toBinary(unsigned char &c) {
  string re;
  unsigned int s = c;
  for (size_t i = 0; i < 8; i++) {
    re = char((s % 2) + '0') + re;
    s /= 2;
  }
  return re;
}

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
    string binary;
    // decode binary
    for(unsigned char c : buffer) {
      binary += toBinary(c);
    }

    unsigned long long i = 0;
    Node *CUR = HT.root;
    while (true) {
      if (CUR->type == "NYT") {
        if (binary[i] == '1') {
          // EOF
          break;
        } else {
          // New symbol
          i++;
          unsigned int w = 0;
          for (size_t j = 0; j < 8; j++) {
            w <<= 1;
            w += (binary[i+j] - '0');
          }
          unsigned char c = w;
          ofile << c;
          Node *NYT = HT.FindNYT(HT.root);
          Node *ins = HT.Find(c, HT.root);
          HT.Insert(c, NYT, ins);
          i += 8;
          CUR = HT.root;
        }
      } else if (CUR->type == "LEAF") {
        // Exist symbol
        ofile << CUR->symbol;
        Node *NYT = HT.FindNYT(HT.root);
        Node *ins = HT.Find(CUR->symbol, HT.root);
        HT.Insert(CUR->symbol, NYT, ins);
        CUR = HT.root;
      } else {
        // Walk the path
        if (binary[i] == '0') {
          CUR = CUR->left;
        } else {
          CUR = CUR->right;
        }
        i++;
      }
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
    string binary;
    for(unsigned char c : buffer) {
      // Insert nodes
      Node *NYT = HT.FindNYT(HT.root);
      Node *ins = HT.Find(c, HT.root);
      if (NYT == NULL) {
        // First insert 0 as symbol
        binary += "0" + toBinary(c);
      } else {
        if (ins == NULL) {
          // Still having symbol, insert 0
          string s = "";
          binary += HT.GetPath(NYT, s);
          binary += "0" + toBinary(c);
        } else {
          // Exist symbol, add path
          string s = "";
          binary += HT.GetPath(ins, s);
        }
      }
      HT.Insert(c, NYT, ins);
    }

    // EOF insert
    string s = "";
    Node *NYT = HT.FindNYT(HT.root);
    binary += HT.GetPath(NYT, s);
    binary += "1";

    // Trailing bits, making 8n bits
    while(binary.size() % 8 != 0) {
      binary += "0";
    }

    // Write binary to file, using ull for huge file
    unsigned long long len = binary.size();
    unsigned int w = 0;
    for (unsigned long long i = 0; i < len; i++) {
      w <<= 1;
      w += (binary[i] - '0');
      if (i != 0 && (i+1) % 8 == 0) {
        ofile << char(w);
        w = 0;
      }
    }
  } else {
    cout << "Read file error" << endl;
  }
  // Close fstream
  ifile.close();
  ofile.close();
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
