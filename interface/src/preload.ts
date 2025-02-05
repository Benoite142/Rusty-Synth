import { contextBridge, ipcRenderer } from 'electron';


// anything ran by scripts imported by the renderer will not have node env
// hence here we can expose functions or props to let the ui interact with the system
// this is in part for safety reason, which means we have to be careful about what we expose
//
// when adding something here, remember to add it to the exposed type
contextBridge.exposeInMainWorld('electronAPI', {
	sendToMainProcess: (arg: string) => ipcRenderer.invoke('boiler-plate-event', arg),
	onUpdateCounter: (callback: (v: number) => void) => ipcRenderer.on('update-counter', (_, value: number) => callback(value))
});
