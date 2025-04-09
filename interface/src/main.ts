import { app, BrowserWindow, ipcMain, Menu } from 'electron';
import * as path from 'path';
import { ConnectionHandler } from './connection_handler';

const isDevEnv = !app.isPackaged;

const getDirPath = (): string => {
  return isDevEnv ? __dirname : process.resourcesPath;
};

const createMainWindow = (): BrowserWindow => {
  const window = new BrowserWindow({
    title: 'rusty synth',
    width: 1600,
    height: 1200,
    webPreferences: {
      // this is dangerous only if fetching on the web
      // since this project is local this is fine
      webSecurity: false,
      preload: path.join(getDirPath(), 'preload.js'),
    },
  });

  if (isDevEnv) {
    window.webContents.openDevTools();
  }

  window.loadFile(path.join(getDirPath(), 'renderer/index.html'));
  return window;
};

app.whenReady().then(() => {
  const win = createMainWindow();

  const connectionHandler = new ConnectionHandler(win);

  const windowMenu: Electron.MenuItemConstructorOptions[] = [
    {
      label: 'Select Devices',
      submenu: [
        {
          label: 'MIDI Device',
          click: () => connectionHandler.sendMessage('select-midi'),
        },
        {
          label: 'Output Device',
          click: () => connectionHandler.sendMessage('select-output'),
        },
      ],
    },
    {
      label: 'Audio Recording',
      submenu: [
        {
          label: 'Start Recording',
          click: () => connectionHandler.sendMessage('start-recording'),
        },
        {
          label: 'Stop Recording',
          click: () => connectionHandler.sendMessage('stop-recording'),
        },
      ],
    },
  ];

  Menu.setApplicationMenu(Menu.buildFromTemplate(windowMenu));

  ipcMain.handle('send-message', (_event, arg: string) => {
    console.log('invoked send message');
    connectionHandler.sendMessage(arg);
  });

  ipcMain.handle('grab-keyboard', (_event) => {
    connectionHandler.sendMessage('start-grabbing-keyboard-input');
  });

  app.on('activate', () => {
    if (BrowserWindow.getAllWindows().length === 0) {
      createMainWindow();
    }
  });

  app.on('window-all-closed', () => {
    connectionHandler.sendMessage('quit');
    // not really necessary since we don't plan on supporting apple devices
    if (process.platform !== 'darwin') {
      app.quit();
    }
  });
});
