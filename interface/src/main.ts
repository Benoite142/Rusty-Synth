import { app, BrowserWindow, ipcMain, Menu } from 'electron';
import * as path from 'path'

const isDevEnv = !app.isPackaged;

const getDirPath = (): string => {
	return isDevEnv ? __dirname : process.resourcesPath;
}

const createMainWindow = () => {
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

	const menu = Menu.buildFromTemplate([
		{
			label: app.name,
			submenu: [
				{
					click: () => window.webContents.send('update-counter', 1),
					label: 'Increment me!'
				}
			],
		}
	]);

	Menu.setApplicationMenu(menu);

	if (isDevEnv) {
		window.webContents.openDevTools();
	}

	window.loadFile(path.join(getDirPath(), 'renderer/index.html'));
}

app.whenReady().then(() => {
	ipcMain.handle('boiler-plate-event', (_, arg: string) => {
		console.log(`received ${arg} from the ui`);
		return 'received your value, chief!';
	});

	createMainWindow();

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

