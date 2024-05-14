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

// Görev kartı veri yapısı
typedef struct TaskCard {
    char task_name[50];
    char task_description[100];
    char due_date[20];
    Userptr assigned_user;
    struct TaskCard *prev;
    struct TaskCard *next;
} TaskCard;
typedef TaskCard* TaskCardPtr;

// Çift yönlü bağlı liste veri yapısı
typedef struct {
    TaskCardPtr head;
    TaskCardPtr tail;
} DoublyLinkedList;
typedef DoublyLinkedList* DoublyLinkedListPtr;

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

// Yeni görev kartı oluşturma fonksiyonu
TaskCardPtr createTaskCard(char *task_name, char *task_description, char *due_date, Userptr assigned_user) {
    TaskCardPtr new_task_card = (TaskCardPtr)malloc(sizeof(TaskCard));
    strcpy(new_task_card->task_name, task_name);
    strcpy(new_task_card->task_description, task_description);
    strcpy(new_task_card->due_date, due_date);
    new_task_card->assigned_user = assigned_user;
    new_task_card->prev = NULL;
    new_task_card->next = NULL;
    return new_task_card;
}

// Görev kartı ekleme fonksiyonu
void addTaskCard(DoublyLinkedListPtr list, TaskCardPtr new_task_card) {
    if (list->head == NULL) {
        list->head = new_task_card;
        list->tail = new_task_card;
    } else {
        list->tail->next = new_task_card;
        new_task_card->prev = list->tail;
        list->tail = new_task_card;
    }
}

// Proje menüsü fonksiyonu
void projectMenu(BPlusTreePtr tree, char *project_name, DoublyLinkedListPtr task_cards) {
    int choice;
    char task_name[50];
    char task_description[100];
    char due_date[20];
    int user_number;

    do {
        printf("--- %s PROJESI ---\n", project_name);
        printf("Görev Kartlari:\n");
        TaskCardPtr current = task_cards->head;
        while (current != NULL) {
            printf("- Görev Adı: %s\n", current->task_name);
            printf("  Görev Açıklaması: %s\n", current->task_description);
            printf("  Teslim Tarihi: %s\n", current->due_date);
            printf("  Atanan Kullanıcı: %s\n", current->assigned_user->username);
            current = current->next;
        }

        printf("\n--- PROJE MENÜSÜ ---\n");
        printf("1. Yeni görev kartı ekle\n");
        printf("2. Tüm kullanıcıları listele\n");
        printf("3. Görev kartına kullanıcı ata\n");
        printf("4. Çıkış yap\n");
        printf("Seçiminizi yapın: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Yeni görev kartı ekranı\n");
                printf("Görev adını girin: ");
                scanf("%s", task_name);
                printf("Görev açıklamasını girin: ");
                scanf("%s", task_description);
                printf("Teslim tarihini girin: ");
                scanf("%s", due_date);
                addTaskCard(task_cards, createTaskCard(task_name, task_description, due_date, NULL)); // Atanan kullanıcıyı sonra ekleyeceğiz
                break;
            case 2:
                printf("Tüm kullanıcılar:\n");
                // Bütün kullanıcıları listele
                // Bu kısmı mevcut kodunuza göre düzenleyebilirsiniz
                break;
            case 3:
                printf("Görev kartına kullanıcı atama ekranı\n");
                printf("Kullanıcı numarasını girin: ");
                scanf("%d", &user_number);
                // Kullanıcıyı görev kartına ata
                // Bu kısmı mevcut kodunuza göre düzenleyebilirsiniz
                break;
            case 4:
                printf("Proje menüsünden çıkılıyor...\n");
                break;
            default:
                printf("Geçersiz seçim! Lütfen tekrar deneyin.\n");

        }

    } while (choice != 4);
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
    char project_name[50];
    DoublyLinkedListPtr task_cards = (DoublyLinkedListPtr)malloc(sizeof(DoublyLinkedList));

    int isLoggedIn = 0; // Kullanıcı girişi yapıldı mı kontrolü

    do {
        if (!isLoggedIn) {
            printf("--- GİRİŞ MENÜSÜ ---\n");
            printf("1. Giriş yap\n");
            printf("2. Kullanıcı ekle\n");
            printf("3. Çıkış yap\n");
            printf("Seçiminizi yapın: ");
            scanf("%d", &choice);

            switch (choice) {
                case 1:
                    printf("Kullanıcı adını girin: ");
                    scanf("%s", input_username);

                    printf("Şifreyi girin: ");
                    scanf("%s", input_password);

                    Userptr user = searchUserInBPlusTree(tree, input_username);

                    if (user != NULL) {
                        if (strcmp(user->password, input_password) == 0) {
                            printf("Giriş başarılı!\n");
                            isLoggedIn = 1; // Giriş yapıldı
                        } else {
                            printf("Hatalı şifre!\n");
                        }
                    } else {
                        printf("Kullanıcı bulunamadı!\n");
                        printf("Yeni kullanıcı eklemek ister misiniz? (e/h): ");
                        // Yeni kullanıcı eklemek isteyip istemediğini al
                        scanf(" %c", &soru);
                        // Satır sonu karakterini temizle
                        while (getchar() != '\n');
                        if (soru == 'e' || soru == 'E') {
                            printf("\nYeni kullanıcı adı girin: ");
                            scanf("%s", new_username);
                            printf("Yeni şifreyi girin: ");
                            scanf("%s", new_password);
                            printf("Numarayı girin: ");
                            scanf("%d", &new_number);
                            // Yeni kullanıcıyı ekle
                            addUserToBPlusTree(tree, new_username, new_password, new_number);
                            printf("Yeni kullanıcı başarıyla eklendi!\n");
                        }
                    }
                    break;

                case 2:
                    printf("\nYeni kullanıcı adı girin: ");
                    scanf("%s", new_username);

                    printf("Yeni şifreyi girin: ");
                    scanf("%s", new_password);

                    printf("Numarayı girin: ");
                    scanf("%d", &new_number);

                    // Yeni kullanıcıyı ekle
                    addUserToBPlusTree(tree, new_username, new_password, new_number);
                    printf("Yeni kullanıcı başarıyla eklendi!\n");
                    break;

                case 3:
                    printf("Çıkış yapılıyor...\n");
                    break;

                default:
                    printf("Geçersiz seçim! Lütfen tekrar deneyin.\n");
            }
        } else {
            printf("--- PROJE MENÜSÜ ---\n");
            printf("Proje adını girin: ");
            scanf("%s", project_name);
            projectMenu(tree, project_name, task_cards); // Proje menüsünü aç
            break; // Proje menüsü tamamlandığında döngüden çık
        }
    } while (choice != 3);

    // Ağacı serbest bırak
    // (Bunu gerçekleştirmek için ağaçta dolaşma ve tüm düğümleri serbest bırakma işlemi yapılmalıdır)

    free(tree);
    free(task_cards);

    return 0;
}
