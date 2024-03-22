#include <memory>

#include "../../include/flow/core/application.hpp"
#include "../../include/flow/core/defines.hpp"

namespace flow {

std::unique_ptr<flow::application> entry(int argc, char** argv);

namespace detail {

    int entry(int argc, char** argv)
    {
        std::unique_ptr<flow::application> app = flow::entry(__argc, __argv);
        app->run();

        return 0;
    }

} // namespace detail
} // namespace flow

#if defined(FLOW_PLATFORM_WINDOWS) && defined(FLOW_RELEASE) && !defined(FLOW_DEBUG)
#  include <windows.h>
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    return flow::detail::entry(__argc, __argv);
}
#else
int main(int argc, char** argv)
{
    return flow::detail::entry(argc, argv);
}
#endif
