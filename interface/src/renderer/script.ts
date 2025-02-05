let counterSpan = document.getElementById('counter');

// button is not on document before on load
// could be used to initialise global values
window.onload = (() => {
	const actionButton = document.getElementById('action-button');
	const valueFromMainSlot = document.getElementById('value-from-main');

	// chances are it did not get picked up initially
	if (!counterSpan) {
		counterSpan = document.getElementById('counter');
	}

	actionButton?.addEventListener('click', async () => {
		const valueFromMain = await window.electronAPI.sendToMainProcess('value from ui');
		valueFromMainSlot.innerText = valueFromMain;
	});
});

window.electronAPI.onUpdateCounter((value: number) => {
	counterSpan.innerText = (Number(counterSpan.innerText) + value).toString();
});
