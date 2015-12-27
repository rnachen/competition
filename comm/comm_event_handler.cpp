/// @file       comm_event_handler.cpp
/// @date       2013/12/04 20:22
///
/// @author
///
/// @brief
///
#include "comm_event_handler.h"
#include "comm_refactor.h"

//构造函数和析构函数
CommEventHandler::CommEventHandler():
    reactor_(NULL),
    event_mask_(NULL_MASK)
{
}

CommEventHandler::CommEventHandler(CommRefactor *reactor):
    reactor_(reactor),
    event_mask_(NULL_MASK)
{
}

CommEventHandler::~CommEventHandler()
{
}

////
int CommEventHandler::handle_input()
{
    return 0;
}

int CommEventHandler::handle_input_ex()
{
    return 0;
}


//
int CommEventHandler::handle_output()
{
    return 0;
}
////
int CommEventHandler::handle_exception()
{
    return 0;
}

int CommEventHandler::handle_close ()
{
    return 0;
}

void CommEventHandler::reactor(CommRefactor *reactor)
{
    reactor_ = reactor;
}

CommRefactor *CommEventHandler::reactor(void) const
{
    return reactor_;
}
