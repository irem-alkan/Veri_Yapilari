#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Task yapısı ve işlevler
struct Task {
    char task_name[100];
    char description[100];
    char due_date[20];
    struct Task* ileri;
    struct Task* onceki;
};
typedef struct Task task;
typedef task* taskptr;

taskptr head = NULL;
taskptr tail = NULL;

void add_task(char task_name[], char description[], char due_date[]) {
    taskptr new_task = (struct Task*)malloc(sizeof(task));
    strcpy(new_task->task_name, task_name);
    strcpy(new_task->description, description);
    strcpy(new_task->due_date, due_date);
    new_task->ileri = NULL;
    new_task->onceki = NULL;

    // Yeni görevi doğru konuma eklemek için teslim tarihine göre sırala
    taskptr current = head;
    while (current != NULL && strcmp(current->due_date, due_date) < 0) {
        current = current->ileri;
    }

    if (current == NULL) {
        // Son elemandan sonra ekle
        if (tail == NULL) {
            head = new_task;
            tail = new_task;
        } else {
            new_task->onceki = tail;
            tail->ileri = new_task;
            tail = new_task;
        }
    } else {
        // Araya veya başa ekle
        if (current->onceki == NULL) {
            // Başa ekle
            new_task->ileri = current;
            current->onceki = new_task;
            head = new_task;
        } else {
            // Araya ekle
            new_task->ileri = current;
            new_task->onceki = current->onceki;
            current->onceki->ileri = new_task;
            current->onceki = new_task;
        }
    }
}

void remove_task(char task_name[]) {
    taskptr current_task = head;
    while (current_task != NULL) {
        if (strcmp(current_task->task_name, task_name) == 0) {
            if (current_task->onceki == NULL) {
                head = current_task->ileri;
                if (head != NULL)
                    head->onceki = NULL;
            } else if (current_task->ileri == NULL) {
                tail = current_task->onceki;
                tail->ileri = NULL;
            } else {
                current_task->onceki->ileri = current_task->ileri;
                current_task->ileri->onceki = current_task->onceki;
            }
            free(current_task);
            return;
        }
        current_task = current_task->ileri;
    }
    printf("Görev '%s' bulunamadı.\n", task_name);
}

void list_tasks() {
    if (head == NULL) {
        printf("Görev bulunamadı.\n");
        return;
    }

    // Görevleri teslim tarihine göre sırala
    taskptr sorted_tasks = head;
    while (sorted_tasks != NULL) {
        taskptr current = sorted_tasks->ileri;
        while (current != NULL) {
            // Tarihlerin karşılaştırılması için gün, ay ve yıl olarak ayırma
            int sorted_day, sorted_month, sorted_year;
            int current_day, current_month, current_year;
            sscanf(sorted_tasks->due_date, "%d.%d.%d", &sorted_day, &sorted_month, &sorted_year);
            sscanf(current->due_date, "%d.%d.%d", &current_day, &current_month, &current_year);

            if (sorted_year > current_year || 
                (sorted_year == current_year && sorted_month > current_month) || 
                (sorted_year == current_year && sorted_month == current_month && sorted_day > current_day)) {
                // Tarihleri takas et
                char temp_name[100];
                char temp_description[100];
                char temp_due_date[20];

                strcpy(temp_name, sorted_tasks->task_name);
                strcpy(temp_description, sorted_tasks->description);
                strcpy(temp_due_date, sorted_tasks->due_date);

                strcpy(sorted_tasks->task_name, current->task_name);
                strcpy(sorted_tasks->description, current->description);
                strcpy(sorted_tasks->due_date, current->due_date);

                strcpy(current->task_name, temp_name);
                strcpy(current->description, temp_description);
                strcpy(current->due_date, temp_due_date);
            }
            current = current->ileri;
        }
        sorted_tasks = sorted_tasks->ileri;
    }

    // Sıralanmış görevleri listele
    taskptr current = head;
    while (current != NULL) {
        printf("Görev Adı: %s\n", current->task_name);
        printf("Açıklama: %s\n", current->description);
        printf("Teslim Tarihi: %s\n\n", current->due_date);
        current = current->ileri;
    }
}

int main() {
    int choice;
    char task_name[100];
    char description[100];
    char due_date[20];

    while (1) {
        printf("\n1. Görev Ekle\n");
        printf("2. Görev Sil\n");
        printf("3. Görevleri Listele\n");
        printf("4. Exit\n");
        printf("Seçiminizi girin: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Görev Adını Giriniz: ");
                scanf("%s", task_name);
                printf("Açıklama Giriniz: ");
                scanf("%s", description);
                printf("Teslim Tarihini Giriniz (dd.mm.yyyy): ");
                scanf("%s", due_date);
                add_task(task_name, description, due_date);
                break;
            case 2:
                printf("Silmek İstediğiniz Görevin Adını Giriniz: ");
                scanf("%s", task_name);
                remove_task(task_name);
                break;
            case 3:
                list_tasks();
                break;
            case 4:
                exit(0);
            default:
                printf("Geçersiz Seçim\n");
        }
    }

    return 0;
}