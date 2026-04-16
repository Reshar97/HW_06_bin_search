#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

/* Структура для хранения данных студента.*/
struct ex_bs_student {
    int id;
    char name[32];
};

/* Отсортированный массив данных. Ключи (id) идут по возрастанию. */
static const struct ex_bs_student ex_bs_students[] = {
    {1, "Anna"},
    {3, "Boris"},
    {5, "Petr"},
    {7, "Galina"},
};
#define EX_BS_N_STUDENTS ARRAY_SIZE(ex_bs_students)

/* Функция бинарного поиска по массиву */
static struct ex_bs_student *ex_bs_binary_search(int id) {
    int left = 0, right = EX_BS_N_STUDENTS - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (ex_bs_students[mid].id == id)
            return (struct ex_bs_student *)&ex_bs_students[mid];
        if (ex_bs_students[mid].id < id)
            left = mid + 1;
        else
            right = mid - 1;
    }
    return NULL;
}

/* Функция для вывода содержимого всего массива */
static void ex_bs_print_all_students(void) {
    int i;
    printk(KERN_INFO "ex_bin_search: --- Содержимое массива студентов ---");
    for (i = 0; i < EX_BS_N_STUDENTS; i++) {
        printk(KERN_INFO "ex_bin_search: Студент [%d]: id=%d, name=%s", i, ex_bs_students[i].id, ex_bs_students[i].name);
    }
    printk(KERN_INFO "ex_bin_search: --- Конец массива ---");
}

static int __init ex_bin_search_init(void) {
    int search_id = 5; // ID студента, которого будем искать

    printk(KERN_INFO "ex_bin_search: Инициализация модуля\n");

    /* Выводим исходный массив данных */
    ex_bs_print_all_students();

    /* Демонстрация поиска */
    printk(KERN_INFO "ex_bin_search: Ищем студента с id %d...\n", search_id);
    
    struct ex_bs_student *found = ex_bs_binary_search(search_id);
    if (found)
        printk(KERN_INFO "ex_bin_search: Найден студент %d (%s)\n", found->id, found->name);

    return 0;
}

static void __exit ex_bin_search_exit(void) {
    printk(KERN_INFO "ex_bin_search: Модуль выгружен\n");
}

module_init(ex_bin_search_init);
module_exit(ex_bin_search_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anton Zubin");
MODULE_DESCRIPTION("Binary Search Example");
