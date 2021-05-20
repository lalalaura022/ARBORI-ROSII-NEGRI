#include <bits/stdc++.h>
using namespace std;

enum COLOR { RED, BLACK }; //retinem culorile posibile

class Node
{
public:
    int val;
    COLOR color;
    Node *left, *right, *parent;

    Node(int val)
    {
        this->val = val;
        parent = left = right = NULL;

        //initializam toate nodurile cu rosu
        color = RED;
    }

    // cautam unchiul pentru a ii returna pointer-ul
    Node *uncle()
    {
        // daca nu exista parinte sau bunic, nu exista nici unchi
        if (parent == NULL || parent->parent == NULL)
            return NULL;

        if (parent->isOnLeft())
            // unchiul din dreapta
            return parent->parent->right;
        else
            // unchiul din stanga
            return parent->parent->left;
    }

    // verificam daca nodul este fiul din stanga
    bool isOnLeft()
    {
        return this == parent->left;
    }

    // ii cautam fratele
    Node *sibling()
    {
        // daca nu exista parinte, nu exista nici frate
        if (parent == NULL)
            return NULL;

        if (isOnLeft())
            return parent->right; //daca nodul nostru este din stanga, fratele sau este in dreapta

        return parent->left; // daca nu este din stanga, fratele sau este in stanga
    }

    // mutam totul de sub mai jos si fixam nodul dat in pozitia sa
    void moveDown(Node *nParent)
    {
        if (parent != NULL) //daca nu este radacina
        {
            if (isOnLeft()) //daca este in stanga
            {
                parent->left = nParent; //il fixam in stanga
            }
            else
            {
                parent->right = nParent;
            }
        }
        nParent->parent = parent; //bunicul devine parinte
        parent = nParent; //parintele devine nodul dat
    }

    bool hasRedChild()
    {
        return ((left != NULL and left->color == RED) ||
                (right != NULL and right->color == RED)); //daca exista un fiu rosu ori in stanga ori in dreapta
    }
};

class RBTree
{
public:
    Node *root;

    // rotire la stanga
    void leftRotate(Node *x)
    {
        // noul parinte va fi copilul drept al nodului
        Node *nParent = x->right;

        // daca noul nod e radacina, schimbam radacina cu parintele
        if (x == root)
            root = nParent;

        x->moveDown(nParent);//fixam pozitia noului parinte

        // facem legatura dintre x si fiul stang al noului nod
        x->right = nParent->left;

        // daca nu este nulll nodul, facem legatura intre elementul stang al noului parinte si nod
        if (nParent->left != NULL)
            nParent->left->parent = x;

        // elementul stang al noului parinte devine x

        nParent->left = x;
    }

    //rotire la dreapta
    void rightRotate(Node *x)
    {
        // noul parinte va fi copilul stang al nodului
        Node *nParent = x->left;

        //modificam radacina in cazul in care nodul e radacina
        if (x == root)
            root = nParent;

        x->moveDown(nParent); //fixam pozitia noului parite

        // procedam exact la felca la rotirea la stanga, dar stanga devine dreapta si invers
        x->left = nParent->right;

        if (nParent->right != NULL)
            nParent->right->parent = x;

        nParent->right = x;
    }

    void swapColors(Node *x1, Node *x2) //functie pentru a schimba culorile intre ele
    {
        COLOR aux;
        aux = x1->color;       //
        x1->color = x2->color; // facem swap ul
        x2->color = aux;       //
    }

    void swapValues(Node *u, Node *v) //functie pentru a schimba valorile intre ele
    {
        int aux;
        aux = u->val;    //
        u->val = v->val; // facem swap ul
        v->val = aux;    //
    }

    // verificam daca nodurile rosii au culoarea ok
    void fixRedRed(Node *x)
    {
        // daca nodul este radacina, il facem negru si ne oprim
        if (x == root)
        {
            x->color = BLACK;
            return;
        }

        // initializam parintele bunicul si unchiul
        Node *parent = x->parent, *grandparent = parent->parent,
              *uncle = x->uncle();

        if (parent->color != BLACK) //daca parintele e negru
        {
            if (uncle != NULL && uncle->color == RED) //daca unchiul e rosu
            {
                // trebuie sa schimbam culorile si sa o luam de la inceput (de la bunic)
                parent->color = BLACK;
                uncle->color = BLACK;
                grandparent->color = RED;
                fixRedRed(grandparent);
            }
            else
            {
                // altfel folosim una dintre rotatii
                if (parent->isOnLeft())
                {
                    if (x->isOnLeft())
                    {
                        swapColors(parent, grandparent); //daca si tatal si nodul sunt in stanga, schimbam culoarea intre parinte si bunic
                    }
                    else
                    {
                        leftRotate(parent);          //daca parintele e in stanga, dar nodul in dreapta
                        swapColors(x, grandparent);  // apelam rotirea in stanga si schimbam culoarea intre nod si bunic
                    }

                    rightRotate(grandparent);
                }
                else
                {
                    if (x->isOnLeft())  //daca tatal e in dreapta si nodul in stanga
                    {
                        rightRotate(parent); //rotim la dreapta
                        swapColors(x, grandparent); //schimbam culoarea intre nod si bunic
                    }
                    else //daca si tatal si nodul sunt in dreapta
                    {
                        swapColors(parent, grandparent); //schimbam culoarea intre parinte si bunic
                    }


                    leftRotate(grandparent);
                }
            }
        }
    }


    //ultimul nod care nu are un fiu sang
    Node *successor(Node *x)
    {
        Node *aux = x;

        while (aux->left != NULL) //cat timp mai exista un fiu stang
            aux = aux->left; //mergem pe latura stanga

        return aux;
    }

    // cautam nodul care poate veni in locul celui sters
    Node *BSTreplace(Node *x)
    {
        // daca nodul are 2 copii
        if (x->left != NULL and x->right != NULL)
            return successor(x->right);
        else
            // daca nodul e frunza
            if (x->left == NULL and x->right == NULL)
                return NULL;
            else
                //daca nodul are doar un copil
                if (x->left != NULL)
                    return x->left;
                else
                    return x->right;
    }

    // stergerea
    void deleteNode(Node *v)
    {
        Node *u = BSTreplace(v); // cautam nodul cu care il putem inlocui pe v(cel pe care urmeaza sa l stergem

        // adevarat daca si u si v sunt negre
        bool uvBlack = ((u == NULL or u->color == BLACK) and (v->color == BLACK));
        Node *parent = v->parent;

        if (u == NULL)
        {
            //daca u este null, inseamna ca v este frunza
            if (v == root)
            {
                root = NULL; //daca v este radacina, pur si simplu facem radacina null
            }
            else
            {
                if (uvBlack) //daca ambele sunt negre
                {
                    fixDoubleBlack(v);
                }
                else// cel putin unul dintre noduri e negru
                {
                    if (v->sibling() != NULL) //daca nodul are un frate, il facem rosu
                        v->sibling()->color = RED;
                }

                if (v->isOnLeft())//daca nodul e in stanga
                {
                    parent->left = NULL; //stergem fiul tatalui din stanga
                }
                else//daca nodul e in dreapta
                {
                    parent->right = NULL;//stergem fiul tatalui din stanga
                }
            }
            delete v;
            return;
        }

        if (v->left == NULL || v->right == NULL)//daca v are maxim un fiu
        {
            if (v == root) //daca v este radacina
            {
                v->val = u->val; //v devine u si il stergem pe u
                v->left = v->right = NULL;
                delete u;
            }
            else
            {

                if (v->isOnLeft())      //
                {
                    //
                    parent->left = u;   //
                }                       //
                else                    // inlocuim v-ul cu u
                {
                    //
                    parent->right = u;  //
                }                       //
                delete v;
                u->parent = parent; //tatal lui u devine tatal initial
                if (uvBlack)
                {
                    fixDoubleBlack(u);
                }
                else
                {
                    // ne asiguram ca u este negru
                    u->color = BLACK;
                }
            }
            return;
        }
        else
            // v has 2 children, swap values with successor and recurse
            swapValues(u, v);
        deleteNode(u);
    }

    void fixDoubleBlack(Node *x) //in cazul in care nodul sters va fi inlocuit tot de unul negru
    {
        if (x == root) //daca e radacina, ne oprim
            return;

        Node *sibling = x->sibling(), *parent = x->parent; //initializam fratele si parintele
        if (sibling == NULL) //daca nu exista frate, continuam urcand la parinte
        {
            fixDoubleBlack(parent);
        }
        else
        {
            if (sibling->color == RED)
            {
                parent->color = RED;
                sibling->color = BLACK;
                if (sibling->isOnLeft()) //daca nodul nostru este in dreapta
                {
                    rightRotate(parent); //rotim spre dreapta
                }
                else//daca nodul nostru este in stanga
                {
                    leftRotate(parent);//rotim spre stanga
                }
                fixDoubleBlack(x);
            }
            else //daca fratele este negru
            {
                if (sibling->hasRedChild())// daca are macar un copil rosu
                {
                    if (sibling->left != NULL && sibling->left->color == RED)//daca cel stang este rosu
                    {
                        if (sibling->isOnLeft()) //daca nodul nostru este in dreapta
                        {
                            sibling->left->color = sibling->color; //copilul din stanga al fratelui ia culoarea fratelui
                            sibling->color = parent->color; //fratele ia culoarea tatalui (este ca o permutare)
                            rightRotate(parent); //rotim spre dreapta de la parinte
                        }
                        else //nodul nostru este in stanga
                        {
                            sibling->left->color = parent->color; //copilul din stanga al fratelui ia culoarea tatalui
                            rightRotate(sibling);//rotim spre dreapta de la frate
                            leftRotate(parent); //rotim spre stanga de la parinte
                        }
                    }
                    else //daca cel drept este rosu
                    {
                        if (sibling->isOnLeft())                     //
                        {
                            //
                            sibling->right->color = parent->color;   //
                            leftRotate(sibling);                     //
                            rightRotate(parent);                     //
                        }                                            //se intampla exact la fel ca inainte, dar se
                        else                                         //inverseaza pozitiile
                        {
                            //
                            sibling->right->color = sibling->color;  //
                            sibling->color = parent->color;          //
                            leftRotate(parent);                      //
                        }
                    }
                    parent->color = BLACK; //culoarea parintelui devine neagra
                }//daca n are niciun copil rosu
                else
                {
                    sibling->color = RED; //unul dintre fii devine rosu
                    if (parent->color == BLACK)
                        fixDoubleBlack(parent);
                    else
                        parent->color = BLACK;
                }
            }
        }
    }

    // folosim parcurgerea in inordine, deoarece ne ajuta la afisarea elementelor in ordine
    void inorder(Node *x)
    {
        if (x == NULL)
            return;
        inorder(x->left);      // parcurgem spre stanga, pentru a lua in ordine crescatoare
        cout << x->val << " ";
        inorder(x->right);
    }


    RBTree() //constructor
    {
        root = NULL;
    }
    Node *getRoot()
    {
        return root;
    }

    Node *search(int n) //functie de cautare
    {
        Node *aux = root; //pornim de la radacina
        while (aux != NULL)
        {
            if (n < aux->val) //daca valoarea cautata este mai mica decat nodul curent
            {
                if (aux->left == NULL) //verificam in stanga si daca am ajuns la frunza ne oprim
                    break;
                else
                    aux = aux->left;//altfel continuam spre stanga
            }
            else if (n == aux->val) //daca am gasit valoarea ne oprim
            {
                break;
            }
            else //daca este mai mare
            {
                if (aux->right == NULL) //verificam in dreapta si se opreste in pozitia in care nu mai are rost sa cautam
                    break;
                else
                    aux = aux->right;//altfel continuam spre dreapta
            }
        }

        return aux; //intr un final valoarea returnata va fi ori n, ori null
    }

    void insert(int n)//functie de inserare
    {
        Node *newNode = new Node(n);
        if (root == NULL) //daca inca nu avem nicio valoare in arbore, pur si simplu adaugam si il facem negru
        {
            newNode->color = BLACK;
            root = newNode;
        }
        else
        {
            Node *aux = search(n); //verificam daca exista nodul

            if (aux->val == n)//daca deja exista, nu facem nimic
            {
                return;
            }

            newNode->parent = aux; //facem legatura intre noul nod si nodul pe care vrem sa l adaugam

            if (n < aux->val)
                aux->left = newNode; //daca este mai mic, il adaugam in stanga
            else
                aux->right = newNode;//daca este mai mare, il adaugam in dreapta

            // verificam sa se indeplineasca toate conditiile
            fixRedRed(newNode);
        }
    }


    void deleteByVal(int n)//functie care sterge
    {
        if (root == NULL) //daca arborele este gol, nu facem nimic
            return;

        Node *v = search(n), *u;

        if (v->val != n) //daca nu s a gasit valoarea
        {
            cout << "Nu a fost gasit niciun nod cu valoarea: " << n << endl;
            return;
        }
        else
            deleteNode(v); //daca a fost gasit nod, il stergem
    }

    void printInOrder()//functie pentru a afisa parcurgerea in inordine
    {
        cout << "Inorder: " << endl;
        if (root == NULL)
            cout << "Tree is empty" << endl;
        else
            inorder(root);
        cout << endl;
    }
};

void predsucc(Node* root, Node*& pre, Node*& suc, int key)
{
    //  daca nu este nimic in arbore nu se intampla nimic
    if (root == NULL)  return ;

    // daca cheia este radacina
    if (root->val == key)
    {
        // predecesorul va fi cea mai mare valoare din subarborele stang
        if (root->left != NULL) //daca exista fiu stang
        {
            Node* aux = root->left;
            while (aux->right) //cat timp exista fiu in dreapta mergem in dreapta
                aux = aux->right;
            pre = aux ;
        }

        // succesorul va fi cea mai mica valoare din dreapta
        if (root->right != NULL)
        {
            Node* aux = root->right ;
            while (aux->left) //cat timp exista fiu in stanga mergem in stanga
                aux = aux->left ;
            suc = aux ;
        }
        return ;
    }

    // daca cheia are valoarea mai mica decat radacina, parcurgem subarborele stang
    if (root->val> key)
    {
        suc = root ;
        predsucc(root->left, pre, suc, key) ;
    }
    else // daca cheia are valoarea mai mare decat radacina, parcurgem subarborele drept
    {
        pre = root ;
        predsucc(root->right, pre, suc, key) ;
    }
}
int main()
{
    RBTree tree;
    int x,n, i, j;;

    tree.insert(7);
    tree.insert(3);
    tree.insert(18);
    tree.insert(10);
    tree.insert(22);
    tree.insert(8);
    tree.insert(11);
    tree.insert(26);
    tree.insert(2);
    tree.insert(6);
    tree.insert(13);

    tree.printInOrder();


    cout<<endl<<"Deleting 18, 11, 3, 10, 22"<<endl;

    tree.deleteByVal(18);
    tree.deleteByVal(11);
    tree.deleteByVal(3);
    tree.deleteByVal(10);
    tree.deleteByVal(22);

    tree.printInOrder();

    Node* pre = NULL, *suc = NULL;
    int key;
    cin>>key;
    predsucc(tree.root, pre, suc, key);
    if (pre != NULL)
        cout << "Predecessor is " << pre->val << endl;
    else
        cout << "No Predecessor";

    if (suc != NULL)
        cout << "Successor is " << suc->val;
    else
        cout << "No Successor";


    return 0;
}
