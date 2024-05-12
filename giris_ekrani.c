#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Kullanıcı veri yapısı
typedef struct {
    char username[20];
    char password[20];
    int number;
} User;
typedef User* Userptr;

// B+ ağacı düğümü
typedef struct BPlusNode {
    int num_keys;
    char **keys;
    Userptr *users;
    struct BPlusNode **pointers;
    struct BPlusNode *parent;
    int is_leaf;
    struct BPlusNode *next; // İleriye dönük bağlantı (leaf düğümleri için)
} BPlusNode;
typedef BPlusNode* BPlusNodePtr;

// B+ ağacı
typedef struct {
    BPlusNodePtr root;
} BPlusTree;
typedef BPlusTree* BPlusTreePtr;

// B+ ağacı oluşturma fonksiyonu
BPlusTreePtr createBPlusTree() {
    BPlusTreePtr tree = (BPlusTreePtr)malloc(sizeof(BPlusTree));
    tree->root = NULL;
    return tree;
}

// Yeni kullanıcı ekleme fonksiyonu
void addUserToBPlusTree(BPlusTreePtr tree, char *username, char *password, int number) {
    // Ağaçta yeni bir düğüm oluştur
    if (tree->root == NULL) {
        tree->root = (BPlusNodePtr)malloc(sizeof(BPlusNode));
        tree->root->num_keys = 0;
        tree->root->keys = NULL;
        tree->root->users = NULL;
        tree->root->pointers = NULL;
        tree->root->parent = NULL;
        tree->root->is_leaf = 1;
        tree->root->next = NULL;
    }
    // B+ ağacında kullanıcı adı sıralı olduğu için sadece yaprak düğümlere ekleyeceğiz
    BPlusNodePtr leaf_node = tree->root;
    while (!leaf_node->is_leaf) {
        // Ağaçta yaprak düğümüne kadar in
        // (Bu örnekte, ağaç sadece bir seviyede olduğundan, yaprak düğümünü doğrudan root olarak alıyoruz)
        leaf_node = leaf_node->pointers[0];
    }

    // Yaprak düğümünde yeni bir kullanıcı ekle
    if (leaf_node->num_keys < 3) { // Düğüm kapasitesi 3
        // Yeni kullanıcıyı eklemek için bellek ayır
        char **new_keys = (char **)malloc((leaf_node->num_keys + 1) * sizeof(char *));
        Userptr *new_users = (Userptr *)malloc((leaf_node->num_keys + 1) * sizeof(Userptr));
        for (int i = 0; i < leaf_node->num_keys; i++) {
            new_keys[i] = leaf_node->keys[i];
            new_users[i] = leaf_node->users[i];
        }
        new_keys[leaf_node->num_keys] = strdup(username);
        Userptr new_user = (Userptr)malloc(sizeof(User));
        strcpy(new_user->username, username);
        strcpy(new_user->password, password);
        new_user->number = number;
        new_users[leaf_node->num_keys] = new_user;

        // Eski belleği serbest bırak
        free(leaf_node->keys);
        free(leaf_node->users);

        leaf_node->keys = new_keys;
        leaf_node->users = new_users;
        leaf_node->num_keys++;
    } else {
        // Yaprak düğümü doluysa, ağacı büyüt
        // (Bu örnekte ağacın büyütülmesi gerektiğini göz ardı ediyoruz)
        // İpucu: Dolu yaprak düğümlerinde ağacı büyütmek için ayırma ve birleştirme işlemleri yapılır.
    }
}

// Kullanıcı arama fonksiyonu
Userptr searchUserInBPlusTree(BPlusTreePtr tree, char *username) {
    if (tree == NULL || tree->root == NULL) {
        return NULL;
    }

    BPlusNodePtr leaf_node = tree->root;
    while (!leaf_node->is_leaf) {
        // Ağaçta yaprak düğümüne kadar in
        // (Bu örnekte, ağaç sadece bir seviyede olduğundan, yaprak düğümünü doğrudan root olarak alıyoruz)
        leaf_node = leaf_node->pointers[0];
    }

    // Yaprak düğümünde kullanıcıyı ara
    for (int i = 0; i < leaf_node->num_keys; i++) {
        if (strcmp(leaf_node->keys[i], username) == 0) {
            return leaf_node->users[i];
        }
    }

    return NULL;
}

int addProject() {
    printf("Proje ekranı\n");
    // Proje ekranı fonksiyonu buraya gelecek
    return 1; // Fonksiyon çağrıldıysa 1 döndür
}

int main() {
    BPlusTreePtr tree = createBPlusTree();

    char input_username[20];
    char input_password[20];
    char new_username[20];
    char new_password[20];
    int new_number;
    char soru;
    int choice;
    int project_added = 0; // Bayrak

    do {
        printf("--- MENU ---\n");
        printf("1. Giris yap\n");
        printf("2. Kullanici ekle\n");
        printf("3. Cikis yap\n");
        printf("Seciminizi yapin: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Kullanici adini girin: ");
                scanf("%s", input_username);

                printf("Sifreyi girin: ");
                scanf("%s", input_password);

                Userptr user = searchUserInBPlusTree(tree, input_username);

                if (user != NULL) {
                    if (strcmp(user->password, input_password) == 0) {
                        printf("Giris basarili!\n");
                        project_added = addProject(); // Fonksiyon çağrıldı mı kontrolü
                        if (project_added) return 0; // Eğer çağrıldıysa programı sonlandır
                    } else {
                        printf("Hatali sifre!\n");
                    }
                } else {
                    printf("Kullanici bulunamadi!\n");
                    printf("Yeni kullanici eklemek ister misiniz? (e/h): ");
                    // Yeni kullanıcı eklemek isteyip istemediğini al
                    scanf(" %c", &soru);
                    // Satır sonu karakterini temizle
                    while (getchar() != '\n');
                    if (soru == 'e' || soru == 'E') {
                        printf("\nYeni kullanici adi girin: ");
                        scanf("%s", new_username);
                        printf("Yeni sifreyi girin: ");
                        scanf("%s", new_password);
                        printf("Numarayi girin: ");
                        scanf("%d", &new_number);
                        // Yeni kullanıcıyı ekle
                        addUserToBPlusTree(tree, new_username, new_password, new_number);
                        printf("Yeni kullanici basariyla eklendi!\n");
                        project_added = addProject(); // Fonksiyon çağrıldı mı kontrolü
                        if (project_added) return 0; // Eğer çağrıldıysa programı sonlandır
                    }
                }
                break;

            case 2:
                printf("\nYeni kullanici adi girin: ");
                scanf("%s", new_username);

                printf("Yeni sifreyi girin: ");
                scanf("%s", new_password);

                printf("Numarayi girin: ");
                scanf("%d", &new_number);

                // Yeni kullanıcıyı ekle
                addUserToBPlusTree(tree, new_username, new_password, new_number);
                printf("Yeni kullanici basariyla eklendi!\n");
                break;

            case 3:
                printf("Programdan cikiliyor...\n");
                return 0;

            default:
                printf("Gecersiz secim! Lutfen tekrar deneyin.\n");
        }
    } while (choice != 3);

    // Ağacı serbest bırak
    // (Bunu gerçekleştirmek için ağaçta dolaşma ve tüm düğümleri serbest bırakma işlemi yapılmalıdır)

    free(tree);

    return 0;
}
