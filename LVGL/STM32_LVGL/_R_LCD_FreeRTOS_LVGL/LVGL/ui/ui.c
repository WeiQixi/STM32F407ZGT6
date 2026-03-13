#include "ui.h"

lv_subject_t temp_subject;

// (1) observer参数：同一个回调函数可以应用到多个观察者 通过第一个参数的地址表示当前触发回调函数的是哪一个观察者
// (2) subject参数：表示当前触发回调函数的主题 =》对应观察的对象
void temp_obs_cb(lv_observer_t * observer, lv_subject_t * subject){
  printf(">>> ? TEMP_OBS_CB TRIGGERED! <<<\n");
  int32_t prev_val = lv_subject_get_previous_int(subject);      // 获取上一个值
  int32_t cur_val = lv_subject_get_int(subject);                // 获取当前值
  printf("temp_obs_cb: %d  prev_val: %d\n", cur_val,prev_val);
}

void add_event_cb(lv_event_t * e){
  lv_subject_t * subject = lv_event_get_user_data(e);
  int32_t cur_val = lv_subject_get_int(subject);      // 获取当前值
  lv_subject_set_int(subject,cur_val+1);

  printf("add_event_cb: %d\n", cur_val);
}

void sub_event_cb(lv_event_t * e){
  lv_subject_t * subject = lv_event_get_user_data(e);
  int32_t cur_val = lv_subject_get_int(subject);      // 获取当前值
  lv_subject_set_int(subject,cur_val-1);

  printf("sub_event_cb: %d\n", cur_val);
}

void ui_init(void){
  // 创建
  lv_obj_t* tabview=lv_tabview_create(lv_screen_active());
  lv_obj_set_size(tabview,240,320);

  // 创建tab页
  lv_obj_t* obs_tab=lv_tabview_add_tab(tabview,"OBS");
  lv_obj_t* tab2=lv_tabview_add_tab(tabview,"Tab 2");

  // 添加对应组件到OBS
  // 外部参数的观察者模式
  // 初始化一个主题
  lv_subject_init_int(&temp_subject,10);
  // 添加外部参数的观察者
  lv_subject_add_observer(&temp_subject,temp_obs_cb,NULL);

  // 创建两个按钮
  lv_obj_t* add_temperature=lv_btn_create(obs_tab);
  lv_obj_set_size(add_temperature,50,50);
  lv_obj_align(add_temperature,LV_ALIGN_CENTER,0,0);
  lv_obj_set_style_bg_image_src(add_temperature,LV_SYMBOL_PLUS,0);
  lv_obj_add_event_cb(add_temperature,add_event_cb,LV_EVENT_CLICKED,&temp_subject);
  lv_obj_t* sub_temperature=lv_btn_create(obs_tab);
  lv_obj_set_size(sub_temperature,50,50);
  lv_obj_align(sub_temperature,LV_ALIGN_CENTER,0,50);
  lv_obj_set_style_bg_image_src(sub_temperature,LV_SYMBOL_MINUS,0);
  lv_obj_add_event_cb(sub_temperature,sub_event_cb,LV_EVENT_CLICKED,&temp_subject);

  // 创建一个标签 用于显示当前温度
  lv_obj_t* temp_label=lv_label_create(obs_tab);
  lv_obj_align(temp_label,LV_ALIGN_TOP_MID,0,20);
  // 将观察者模式绑定到标签组件上  =》 当主题的状态发生变化时，会自动调用回调函数更新标签的显示
  lv_label_bind_text(temp_label,&temp_subject,"temp: %d");

  // 创建一个滑块 =》 实现双向绑定
  lv_obj_t* temp_slider=lv_slider_create(tab2);
  lv_obj_set_size(temp_slider,200,20);
  lv_obj_align(temp_slider,LV_ALIGN_TOP_MID,0,50);
  // 将滑块组件绑定到主题上 =》 当滑块的值发生变化时，会自动更新主题的状态
  lv_slider_bind_value(temp_slider,&temp_subject);
}
