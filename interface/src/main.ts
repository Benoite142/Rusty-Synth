import { app, BrowserWindow, ipcMain, Menu } from 'electron';
import * as path from 'path'

const isDevEnv = process.env.NODE_ENV !== 'production'

const createMainWindow = () => {
	const window = new BrowserWindow({
		width: 1600,
		height: 1200,
		webPreferences: {
			preload: path.join(__dirname, 'preload.js'),
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

	// since we are running from dist (compiled to js)
	// we need to specify the correct path
	window.loadFile('../src/renderer/index.html');
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

