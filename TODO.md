# Minor

- Fix some apps not showing and some not executing
- Add descriptions in help message


# Major

- Add image mode
- Design a logo


---


# Latest changelog (v2.0.0)

- WENT FULLY DAEMON MODE. It's very lightweight and the window launches are now near-instant. Use `--daemon` to start the daemon.
- window/shell launches now just talk to the daemon via an Unix socket, skipping the heavy Qt initialization overhead each time; making the winodw launch almost instantly.
- Removed dmenu mode (`-d, --dmenu`). Please use provider scripts.
- Removed `-dc, --display-column`. Now use `--field`, it's more powerful. Example: `--field name:1` or `--field description:2`