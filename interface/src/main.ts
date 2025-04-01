import { app, BrowserWindow, ipcMain } from 'electron';
import * as path from 'path'
import { ConnectionHandler } from './connection_handler';

const isDevEnv = !app.isPackaged;

const getDirPath = (): string => {
	return isDevEnv ? __dirname : process.resourcesPath;
}

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
		}
	});

	if (isDevEnv) {
		window.webContents.openDevTools();
	}

	window.loadFile(path.join(getDirPath(), 'renderer/index.html'));
	return window;
}

app.whenReady().then(() => {
	const win = createMainWindow();

	const connectionHandler = new ConnectionHandler(win);

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

});

app.on('window-all-closed', () => {
	// not really necessary since we don't plan on supporting apple devices
	if (process.platform !== 'darwin') {
		app.quit();
	}
});

