# Hardware Simulation Debugger Sample

A Qt Widgets + C++ interview sample for a hardware simulation debugger. The application focuses on practical architecture rather than heavy abstraction.

## What it demonstrates

- Qt `QMainWindow` desktop UI with a summary dashboard
- Four data views:
  - Design hierarchy (`QTreeView`)
  - Signal hierarchy (`QTreeView`)
  - Assertion results (`QTableView`)
  - Bus transactions (`QTableView`)
- SQLite database populated with large synthetic demo data
- Lazy loading for both tree and table models
- Explicit observer pattern on top of Qt signal/slot handling via `AppEventBus` and `IAppObserver`
- Background-thread orchestration for long-running RMB actions with status-bar progress
- A lightweight `ServiceManager` coordinating UI-facing services

## Architecture

- `DatabaseManager`: creates schema, seeds large demo data, and serves query helpers
- `BasicTreeViewModel`: lazy hierarchical model backed by SQLite
- `BasicTableViewModel`: batch-loading table model backed by SQLite
- `AppEventBus`: observer subject + Qt signal bridge for progress and dataset events
- `BasicTreeView` / `BasicTableView`: context-menu driven views with threaded actions
- `DashboardWidget`: summary metrics from the database
- `MainWindow`: composes the UI without over-layering the application

## Build

### Requirements

- Qt 5 with Widgets and SQL modules
- CMake 3.16+
- A compiler with C++17 support

### Configure and build

```powershell
cmake -S . -B build -G "Ninja" -DCMAKE_PREFIX_PATH="C:/Qt/5.x.x/msvc2019_64"
cmake --build build
```

If you use Visual Studio or MinGW, adjust `CMAKE_PREFIX_PATH` to your Qt installation.

## Notes for interview discussion

- The observer pattern is intentionally explicit even though Qt signals and slots already provide pub/sub behavior.
- Tree models only fetch child batches on demand, while table models fetch rows in chunks.
- Tree expand/collapse UI updates still occur on the GUI thread, while worker threads drive action progress and pacing.
- The database is generated on first run in the user app-data directory.
