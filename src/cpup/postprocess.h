#pragma once

#include "canis.h"

i32 InitPostProcess(AppContext* _appContext);
void FreePostProcess(AppContext* _appContext);
i32 ResizePostProcess(AppContext* _appContext, i32 _width, i32 _height);
void BeginPostProcess(AppContext* _appContext);
void EndPostProcess(AppContext* _appContext);
