import { contextBridge, ipcRenderer } from 'electron';


// anything ran by scripts imported by the renderer will not have node env
// hence here we can expose functions or props to let the ui interact with the system
// this is in part for safety reason, which means we have to be careful about what we expose
//
// when adding something here, remember to add it to the exposed type
contextBridge.exposeInMainWorld('electronAPI', {
	endKeyboardGrab: (callback: () => void) => ipcRenderer.on('end-keyboard-grab', () => callback()),
	selectDeviceName: (callback: (args: string[]) => void) => ipcRenderer.on('select-device', (_event, args: string[]) => callback(args)),
	sendMessage: (message: string) => ipcRenderer.invoke('send-message', message),
	grabKeyboard: () => ipcRenderer.invoke('grab-keyboard')
});
