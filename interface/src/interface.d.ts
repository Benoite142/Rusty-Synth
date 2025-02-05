export interface IMyAPI {
	sendToMainProcess: (arg: string) => Promise<string>
	onUpdateCounter: (callback: (n: number) => void) => void
}

declare global {
	interface Window {
		electronAPI: IMyAPI
	}
}
