cmake_minimum_required(VERSION 3.16)
project(HardwareSimulationDebugger LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql Charts)

add_executable(HardwareSimulationDebugger
    src/main.cpp
    src/core/AppEventBus.cpp
    src/core/ServiceManager.cpp
    src/core/DatabaseManager.cpp
    src/core/ThemeManager.cpp
    src/repositories/AssertionRepository.cpp
    src/repositories/DesignRepository.cpp
    src/repositories/SignalRepository.cpp
    src/repositories/TransactionRepository.cpp
    src/data_models/BasicTableViewModel.cpp
    src/data_models/BasicTreeViewModel.cpp
    src/data_models/SortFilterProxyModel.cpp
    src/data_models/TransactionTableViewModel.cpp
    src/services/AssertionViewService.cpp
    src/services/DashboardService.cpp
    src/services/DesignViewService.cpp
    src/services/SignalViewService.cpp
    src/services/TransactionViewService.cpp
    src/ui/AssertionView.cpp
    src/ui/BasicTableView.cpp
    src/ui/BasicTreeView.cpp
    src/ui/DashboardWidget.cpp
    src/ui/DashboardSummaryChartWidget.cpp
    src/ui/DesignView.cpp
    src/ui/MainWindow.cpp
    src/ui/SelectionDetailsWidget.cpp
    src/ui/SignalView.cpp
    src/ui/TransactionView.cpp
    src/workers/DataLoadingWorker.cpp
    src/export/CsvExportStrategy.cpp
    src/export/TxtExportStrategy.cpp
    src/export/JsonExportStrategy.cpp
    src/export/HtmlExportStrategy.cpp
    src/export/DataExporter.cpp
)

target_include_directories(HardwareSimulationDebugger
    PRIVATE
        src
        src/core
        src/data_models
        src/export
        src/repositories
        src/services
        src/ui
        src/workers
)

target_link_libraries(HardwareSimulationDebugger
    PRIVATE
        Qt6::Core
        Qt6::Widgets
    Qt6::Charts
        Qt6::Sql
)

if(WIN32)
    set_target_properties(HardwareSimulationDebugger PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
