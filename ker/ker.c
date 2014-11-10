#include<linux/init.h>
#include<linux/module.h>
#include<linux/sched.h>
#include<linux/slab.h>
#include<linux/string.h>
#include<linux/list.h>
typedef struct list_head list_head;
typedef struct task_struct task_struct;
void printTree(task_struct *start, int *pre, int prenum , int offset){
        task_struct * pcb = start;
	list_head *c = &(start ->children);
	int namelen = strlen(pcb->comm);
	printk("%s",pcb->comm);
        if(c->next != c && c->next->next == c){//has only one child
                printk("───");
                printTree(list_entry(c->next, task_struct, sibling), pre, prenum, offset + namelen + 3);
        }else if(!list_empty(c)){//has two more children

		list_head *it;
		list_for_each(it, c){
                        int *t = (int*)kmalloc(sizeof(int)*(prenum+1), GFP_KERNEL);
                        int i;
			for( i = 0; i < prenum; i ++)
                                t[i] = pre[i];
                        t[prenum] = offset + namelen + 1;
                        if(it == c ->next){
                                printk("─┬─");
                                printTree(list_entry(it, task_struct, sibling), t, prenum + 1, offset + namelen + 3);
                        }else if(it != c->prev){
                                int index = 0;
				int k;
                                for( k = 0; k < offset + namelen; k++){
                                        if(index < prenum && k == pre[index]){
                                                printk("│");
                                                index ++;
                                        }
                                        else printk(" " );
                                }
                                printk( " ├─");
                                printTree(list_entry(it, task_struct, sibling), t, prenum + 1, offset + namelen + 3);
                        }else{
                                int index = 0;
				int m ;
                                for(m = 0; m < offset + namelen; m++){
                                        if(index < prenum && m == pre[index]){
                                                printk("│");
                                                index ++;
                                        }
                                        else printk(" " );
                                }
                                printk( " └─");
                                printTree(list_entry(it, task_struct, sibling), pre, prenum, offset + namelen + 3);
                        }
                        if(t != NULL){
                                kfree(t);
                                t = NULL;
                        }
                }
        }else{
                printk("\n");
        }
}

static int hello_init(void){
    //    printk(KERN_WARNING "Module init:Hello word!\n");
       	struct task_struct *root;
        for(root = current; root != &init_task; root = root->parent);// find the root
      	printk(KERN_WARNING "guoruijun's pstree in kernel is staring*************************");
	printTree(root, NULL, 0, 0);
	printk(KERN_WARNING "guoruijun's pstree in kernel has done***************************");
        return 0;
}




static void hello_exit(void){
	printk(KERN_WARNING "Modul exit:Bye-Bye\n");
}

module_init(hello_init);
module_exit(hello_exit);
