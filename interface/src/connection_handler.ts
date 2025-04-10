import { BrowserWindow } from 'electron';
import { createConnection, Socket } from 'net';


export class ConnectionHandler {
	private client: Socket;
	private isCollectingDeviceNames = false;
	private firstMessage = true;

	public constructor(window: BrowserWindow, onErr: () => void) {
		const socket = createConnection({ port: 8000, host: 'localhost' }, () => { });

		const deviceNames: string[] = [];

		socket.on('data', async (data: Buffer) => {
			// when splitting in the start of a string
			// it returns an empty string and the rest of the string
			// so we manually filter out the empty messages
			const stringifiedData = data.toString().split('%$').filter((m: string) => !!m);

			if (this.firstMessage) {
				// timeout to make sure front end is initialized
				// might need to readjust if it still fails sometimes
				await new Promise((res) => setTimeout(res, 1000));
				this.firstMessage = false;
			}

			for (const message of stringifiedData) {
				if (message === 'select devices') {
					this.isCollectingDeviceNames = true;
					continue;
				}

				if (message === 'end of select devices') {
					this.isCollectingDeviceNames = false;
					console.log('end of device selection');
					window.webContents.send('select-device', deviceNames);
					break;
				}

				if (this.isCollectingDeviceNames) {
					console.log('device name:', message);
					deviceNames.push(message);
				}

				if (message === 'end-keyboard-grabbing') {
					console.log('end of keyboard grab');
					// the sent value does not really matter
					// all that matters is we wake up from grabbing
					window.webContents.send('end-keyboard-grab');
				}

			}
		});

		socket.on('error', (err) => {
			console.log('error with socket', err);
			socket.end();
			onErr();
		});

		this.client = socket;
	}

	public sendMessage = (message: string) => {
		console.log('sending message', message);
		this.client.write(message);
	}
}

