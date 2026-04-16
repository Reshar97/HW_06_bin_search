#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/rbtree.h>
#include <linux/slab.h>

struct ex_bt_user {
    int user_id;
    char name[32];
    struct rb_node node;
};

static struct rb_root my_tree = RB_ROOT;

/* Функция поиска пользователя по ID */
static struct ex_bt_user *ex_bt_search_user(int id) {
    struct rb_node *node = my_tree.rb_node;
    while (node) {
        struct ex_bt_user *data = container_of(node, struct ex_bt_user, node);
        if (id < data->user_id)
            node = node->rb_left;
        else if (id > data->user_id)
            node = node->rb_right;
        else
            return data;
    }
    return NULL;
}

/* Функция вставки нового пользователя в дерево */
static void ex_bt_insert_user(int id, const char *name) {
    struct ex_bt_user *data = kmalloc(sizeof(*data), GFP_KERNEL);
    if (!data) {
        printk(KERN_ERR "ex_bin_tree: Не удалось выделить память\n");
        return;
    }
    data->user_id = id;
    snprintf(data->name, sizeof(data->name), "%s", name);

    struct rb_node **new = &(my_tree.rb_node), *parent = NULL;

    /* Идем по дереву, чтобы найти место для вставки */
    while (*new) {
        struct ex_bt_user *this = container_of(*new, struct ex_bt_user, node);
        parent = *new;
        if (id < this->user_id)
            new = &((*new)->rb_left);
        else if (id > this->user_id)
            new = &((*new)->rb_right);
        else {
            printk(KERN_ERR "ex_bin_tree: Пользователь с id %d уже существует\n", id);
            kfree(data);
            return;
        }
    }

    /* Вставляем новый узел и восстанавливаем свойства Красно-Черного дерева */
    rb_link_node(&data->node, parent, new);
    rb_insert_color(&data->node, &my_tree);
    printk(KERN_INFO "ex_bin_tree: Добавлен пользователь с id %d (%s)\n", id, name);
}

/* Функция удаления пользователя из дерева */
static void ex_bt_delete_user(int id) {
    struct ex_bt_user *data = ex_bt_search_user(id);
    if (!data) {
        printk(KERN_INFO "ex_bin_tree: Пользователь %d не найден\n", id);
        return;
    }
    rb_erase(&data->node, &my_tree);
    kfree(data);
    printk(KERN_INFO "ex_bin_tree: Удалён пользователь с id %d\n", id);
}

/* Функция для вывода всех пользователей в дереве (обход in-order) */
static void print_all_users(void) {
    struct rb_node *node;
    printk(KERN_INFO "ex_bin_tree: --- Список всех пользователей в дереве (in-order) ---");
    for (node = rb_first(&my_tree); node; node = rb_next(node)) {
        struct ex_bt_user *data = container_of(node, struct ex_bt_user, node);
        printk(KERN_INFO "ex_bin_tree: Пользователь id: %d, Имя: %s", data->user_id, data->name);
    }
    printk(KERN_INFO "ex_bin_tree: --- Конец списка ---");
}

/* Функция инициализации модуля (вызывается при insmod) */
static int __init ex_bin_tree_init(void) {
    int search_id = 20; // ID пользователя, которого будем искать

    printk(KERN_INFO "ex_bin_tree: Инициализация модуля\n");

    /* Добавляем пользователей и выводим состояние дерева после каждой операции */
    ex_bt_insert_user(10, "Ivan");
    print_all_users();

    ex_bt_insert_user(20, "Olga");
    print_all_users();

    ex_bt_insert_user(5, "Petr");
    print_all_users();

    /* Демонстрация поиска */
    printk(KERN_INFO "ex_bin_tree: Ищем пользователя с id %d...\n", search_id);
    struct ex_bt_user *found = ex_bt_search_user(search_id);
    if (found)
        printk(KERN_INFO "ex_bin_tree: Найден пользователь c id %d (%s)\n", found->user_id, found->name);

    /* Демонстрация удаления */
    printk(KERN_INFO "ex_bin_tree: Удаляем пользователя с id 10\n");
    ex_bt_delete_user(10);
    print_all_users();

    return 0;
}

/* Функция выхода (вызывается при rmmod) */
static void __exit ex_bin_tree_exit(void) {
    struct rb_node *node;
    /* Безопасно удаляем все узлы из дерева при выгрузке модуля */
    for (node = rb_first(&my_tree); node; node = rb_next(node)) {
        struct ex_bt_user *data = container_of(node, struct ex_bt_user, node);
        rb_erase(node, &my_tree);
        kfree(data);
    }
    printk(KERN_INFO "ex_bin_tree: Модуль выгружен\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anton Zubin");
MODULE_DESCRIPTION("Red-Black Tree Example");

module_init(ex_bin_tree_init);
module_exit(ex_bin_tree_exit);
