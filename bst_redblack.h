//bst red black

#ifndef bst_red_black_h
#define bst_red_black_h

#include <cassert> 
#include <climits>
#include <cstring>

template <typename Key, typename Value>

class bst_red_black {
public:
    struct node{
        node(const Key& key, const Value& val, bool color=BLACK,  node* left=nullptr, node* right=nullptr)
        : key_(key), val_(val), color_(color), left_(left), right_(right), count_(0) { }
        
        Key key_;
        Value val_;
        bool color_;
        node* left_, *right_; 
        size_t count_;

        friend std::ostream& operator<<(std::ostream& os, const node& no) {
            return os << no.left_ << "<--" << no.key_ << "," << no.val_ << "(" << &no << ") -->" << no.right_ << "\n";
        }
    };


template <typename T>
class iterator {
    private:
    array_queue<T>* qu_;
    typename array_queue<T>::iterator it_;

    public:
    iterator() : qu_(new array_queue<Key>()), it_(qu_->begin()) {

    }
    iterator& operator++() {
        ++it_;
        }

    iterator& operator++(int) {
        it_++;
        }

    T& operator*() { 
        return *it_; 
        }

    bool operator==(const iterator& other) {
        return it_ == other.it_ && qu_ == other.qu_;
        }

    bool operator!=(const iterator& other) {
        return !operator==(other); 
        }
    
};

private: 
    node* root_;
    const comparator<Key>& comp_;
    static const bool RED = true;
    static const bool BLACK = false;

public:
    bst_red_black(const comparator<Key>& comp = fwd_comparator<Key>()): root_(nullptr), comp_(comp) {

     }

    iterator<Key> keys(bool reverse=false) {
        array_queue<Key> qu = new array_queue<Key>();
        reverse ? revorder(root_, qu) : inorder(root_, qu);
        return iterator(qu);
    }

    void inorder(node* p, array_queue<Key>* q){
        if (p == nullptr) {
            return;
            }
        inorder(p->left_, q);
        q->enqueue(p->key_);
        inorder(p->right, q);
    }

    void revorder(node* p, array_queue<Key>* q){
        if (p==nullptr){
            return;
        }
        revorder(p->right, q);
        q->enqueue(p->key_);
        revorder(p->left_);
    }

    void put(Key key, Value val) {
        root_ = put(root_, key, val); 
        }

    node* put(node* h, Key key, Value val) {
        if (h == nullptr) {
            return new node(key, val, RED); 
            }
        
        int cmp = compare(key, h->key_, comp_);

        if ( cmp < 0) { 
            h->left_ = put(h->left_, key, val);
            } 
         else if (cmp > 0) { 
             h->right_ = put(h->right_, key, val);
         }  
            else { 
                h->val_ = val; 
                }

        if (is_red(h->right_) && !is_red(h->left_)) { 
            h = rotate_left(h); 
                }
        if (is_red(h->left_) && is_red(h->left_->left_)) { 
            h = rotate_right(h); 
                }
        if (is_red(h->left_) && is_red(h->right_)) {
             flip_colors(h); 
                }

        h->count_ = 1 + size(h->left_) + size(h->right_); 
        return h;
    }

    Value get(Key key) {
        check_empty();
        node* p = root_;
        while (p != nullptr) {
            if (less(key, p->key_, comp_)) { 
                p = p->left_;
            } 
                else if (less(p->key_, key, comp_)) { 
                    p = p->right_;
                } 
                    else { 
                        return p->val_;
                            }
        }
        return nullptr;
    }

    void delete_key(Key key) { 
        root_ = delete_key(root_, key);
         }

    node* delete_key(node* p, Key key) {
        if ( p == nullptr) { 
            return nullptr;
            }
        int cmp = compare(key, p->key_);
        if (cmp < 0) {
            p->left_ = delete_key(p->right_, key); 
                } 
            else if(cmp > 0) {
            p->right_ = delete_key(p->right_, key);
                } 
                else {
                    if (p->right_ == nullptr) {
                        return p->left_;
                            }

                    node* subtree = p;
                    p = min(subtree->right_);
                    p->right_ = delete_min(subtree->right_);
                    p->left_ = subtree->left_;
                    }
        p->count_ = size(p->left_) + size(p->right_) + 1;
        return p;
    }

   void delete_min() {
       root_ = delete_min(root_);
        }

   node* delete_min(node* p) {
       if (p->left_ == nullptr) {
           node* right = p->right_;
           delete p;
           return right;
       }
       p->left = delete_min(p->left_);
       p->count_ = 1 + size(p->left_) + size(p->right_);
       return p;
   } 

   size_t size() const { 
       return size(root_); 
       }

   size_t size(node* p) const {
       return p == nullptr? 0: p->count_;
       }

   size_t size(Key low, Key high){
       size_t sz = rank(high) - rank(low);
       return contains(high) ? sz + 1 : sz;

   }
   bool empty() const {   
    return size() == 0; 
    }

   Value min() const{
       check_empty();
       node* p = root_;
       while(p->left_ != nullptr) { p = p->left_; }
       return p->val_;
   }

   size_t rank(Key key) {
       return rank(key, root_);
        }

   size_t rank(Key key, node* p) {
       if (p == nullptr) {
           return 0;
           }
       size_t cmp = compare(key, p->key_);
       if (cmp < 0) {
           return rank(key, p->left_);
          
       } 
         else if (cmp > 0) { 
            return 1 + size(p->left_) + rank(key, p->right);
            } 
            else {
                 return size(p->left_);
                  }
   }
   Key floor(Key key) {
       node* p = floor(root_, key);
       if (p == nullptr) { 
           return nullptr;
           }
       return p->key_;
   }

   node* floor(node* p, Key key) {
       if ( p == nullptr) {
            return nullptr;
            }
       int compare = compare(key, p->key_, comp_ );

       if(compare == 0) { 
           return p;
           }
       if( compare < 0) { 
           return floor(p->left_, key);
           }

       node* subtree = floor(p->right_, key);
       if( subtree != nullptr) { 
           return subtree;
        } 
        else { 
            return p;
            }

   }
    void print_inorder() const {
        print_inorder(root_);
         }
    
    void print_inorder(node* p) const {
        if (p == nullptr) {
            return;
            }
        print_inorder(p->left_); 
        std::cout << *p << "\n"; 
        print_inorder(p->right_);
    }

    // static void run_tests(){
    //     bst_red_black<std::string, std::string>tree;
    //     tree.put("mac_complier", "xcode");
    //     tree.put("win_complier", "vstudio");
    //     tree.put("linux_complier", "G++");
    //     tree.put("all_complier", "clang");
    //     tree.put("pc_complier", "ARM");

    // std::cout << "tree is: " << tree << "\n";
    // }

    friend std::ostream& operator<<(std::ostream& os, const bst_red_black& tree) {
        if (tree.empty()) {
            return os << "bst_red_black is empty \n"; 
            }
        tree.print_inorder();
        return os;
    }

    private:
        void check_empty(){
            if (size() == 0 ) {
                 throw new std::underflow_error("tree is empty\n"); 
                 }
        }

        bool is_red(node* p) {
            return (p == nullptr ? false : (p->color_ == RED)); 
            }

        node* rotate_left(node* h) {
            assert(is_red(h->right_));

            node* p = h->right_;
            h->right_ = p->left_;
            p->left_ = h;
            p->color_ = h->color_;
            h->color_ = RED;
            return p;
        }

        node* rotate_right(node* h) {
            assert(is_red(h->left_));
            
            node* p = h->left_;
            h->left_ = p->right_;
            p->right_ = h;
            p->color_ = h->color_;
            h->color_ = RED;
            return p;
        }
    
        void flip_colors(node* h){
            assert(is_red(h));
            assert(is_red(h->left_));
            assert(is_red(h->right_));
            h->color_ = RED;
            h->left_->color_ = BLACK;
            h->right_->color_ = BLACK;
        }

   };     



#endif     