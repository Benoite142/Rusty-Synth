const MIN_ANGLE = 40; // Min rotation (Bottom-left)
const MAX_ANGLE = 320; // Max rotation (Bottom-right)
const WAVE_FORMS = [
    { value: 'sine', text: 'Sine Wave' },
    { value: 'square', text: 'Square Wave' },
    { value: 'saw', text: 'Saw Wave' },
    { value: 'triangle', text: 'Triangle Wave' },
];

class MenuSelectionDialog {
    private dialog: HTMLDialogElement;
    constructor(dialogBox: HTMLDialogElement) {
        this.dialog = dialogBox;
        this.dialog.addEventListener('keydown', (e) => {
            if (e.key === 'Escape') {
                e.preventDefault();
            }
        });
    }

    public addDialog = <I>(
        selectionTitle: string,
        items: I[],
        callBack: (value: I | number) => void,
        returnType: 'idx' | 'value'
    ): void => {
        // would only want to alter the options and title
        const options = items
            .map((item, idx) => `<option value="${idx}">${item}</option>`)
            .join('');
        this.dialog.setHTMLUnsafe(`<form>
      <p>
        <label>
         ${selectionTitle} 
          <select>
            ${options}
          </select>
        </label>
      </p>
      <div>
        <button id="confirm-btn" value="default">Confirm Selection</button>
      </div>
    </form>`);
        this.dialog.showModal();
        const selector = document.querySelector('select');
        document
            .getElementById('confirm-btn')
            .addEventListener('click', (e) => {
                e.preventDefault();
                callBack(
                    returnType === 'idx'
                        ? (selector.value as any as number)
                        : items[Number(selector.value)]
                );
                this.dialog.close();
            });
    };
}

type Operator = {
    volume: HTMLInputElement | undefined;
    wave_form: {
        left_button: HTMLButtonElement | undefined;
        right_button: HTMLButtonElement | undefined;
        wave_value: HTMLSpanElement | undefined;
    };
    pitch: {
        fine: HTMLInputElement | undefined;
        coarse: HTMLInputElement | undefined;
    };
    adsr: {
        attack: HTMLInputElement | undefined;
        decay: HTMLInputElement | undefined;
        sustain: HTMLInputElement | undefined;
        release: HTMLInputElement | undefined;
    };
    lfo_link: {
        lfo1: HTMLButtonElement | undefined;
        lfo2: HTMLButtonElement | undefined;
    };
};

type LFO = {
    wave_form: {
        left_button: HTMLButtonElement | undefined;
        right_button: HTMLButtonElement | undefined;
        wave_value: HTMLSpanElement | undefined;
    };
    rate: HTMLInputElement | undefined;
    amount: HTMLInputElement | undefined;
};

type GlobalVariables = {
    menu_dialog: MenuSelectionDialog | undefined;
    operators: {
        '1': Operator | undefined;
        '2': Operator | undefined;
        '3': Operator | undefined;
        '4': Operator | undefined;
    };
    lfos: {
        '1': LFO | undefined;
        '2': LFO | undefined;
    };
};

const globalVariables: GlobalVariables = {
    menu_dialog: undefined,
    operators: {
        '1': undefined,
        '2': undefined,
        '3': undefined,
        '4': undefined,
    },
    lfos: { '1': undefined, '2': undefined },
};

const initOperator = (operator_number: number): Operator => {
    const operator = {
        wave_form: {
            wave_value: document.getElementById(
                `wave-type-${operator_number}`
            ) as HTMLSpanElement,
            left_button: document.getElementById(
                `left-wave-selector-${operator_number}`
            ) as HTMLButtonElement,
            right_button: document.getElementById(
                `right-wave-selector-${operator_number}`
            ) as HTMLButtonElement,
        },
        pitch: {
            fine: document.getElementById(
                `pitch-fine-knob-${operator_number}`
            ) as HTMLInputElement,
            coarse: document.getElementById(
                `pitch-coarse-knob-${operator_number}`
            ) as HTMLInputElement,
        },
        adsr: {
            attack: document.getElementById(
                `attack-knob-${operator_number}`
            ) as HTMLInputElement,
            decay: document.getElementById(
                `decay-knob-${operator_number}`
            ) as HTMLInputElement,
            sustain: document.getElementById(
                `sustain-knob-${operator_number}`
            ) as HTMLInputElement,
            release: document.getElementById(
                `release-knob-${operator_number}`
            ) as HTMLInputElement,
        },
        volume: document.getElementById(
            `volume-level-${operator_number}`
        ) as HTMLInputElement,
        lfo_link: {
            lfo1: document.getElementById(
                `lfo1-link-${operator_number}`
            ) as HTMLButtonElement,
            lfo2: document.getElementById(
                `lfo2-link-${operator_number}`
            ) as HTMLButtonElement,
        },
    } as Operator;

    operator.wave_form.right_button?.addEventListener('click', () => {
        const idx = WAVE_FORMS.findIndex(
            (w) => w.text === operator.wave_form.wave_value.textContent
        );
        operator.wave_form.wave_value.setHTMLUnsafe(
            WAVE_FORMS[(idx + 1) % WAVE_FORMS.length].text
        );
        window.electronAPI.sendMessage(
            `op ${operator_number} waveform ${
                WAVE_FORMS[(idx + 1) % WAVE_FORMS.length].value
            }`
        );
    });

    operator.wave_form.left_button?.addEventListener('click', () => {
        const idx = WAVE_FORMS.findIndex(
            (w) => w.text === operator.wave_form.wave_value.textContent
        );
        operator.wave_form.wave_value.setHTMLUnsafe(
            WAVE_FORMS[idx - 1 < 0 ? idx + WAVE_FORMS.length - 1 : idx - 1].text
        );
        console.log(idx - 1 < 0 ? idx + WAVE_FORMS.length - 1 : idx);
        window.electronAPI.sendMessage(
            `op ${operator_number} waveform ${
                WAVE_FORMS[idx - 1 < 0 ? idx + WAVE_FORMS.length - 1 : idx - 1]
                    .value
            }`
        );
    });

    attachKnobListener(operator.pitch.fine, (value) => {
        window.electronAPI.sendMessage(`op ${operator_number} fine ${value}`);
    });

    attachKnobListener(operator.pitch.coarse, (value) => {
        window.electronAPI.sendMessage(`op ${operator_number} coarse ${value}`);
    });

    attachKnobListener(operator.adsr.attack, (value) => {
        window.electronAPI.sendMessage(`op ${operator_number} attack ${value}`);
    });

    attachKnobListener(operator.adsr.decay, (value) => {
        window.electronAPI.sendMessage(`op ${operator_number} decay ${value}`);
    });

    attachKnobListener(operator.adsr.sustain, (value) => {
        window.electronAPI.sendMessage(
            `op ${operator_number} sustain ${value}`
        );
    });

    attachKnobListener(operator.adsr.release, (value) => {
        window.electronAPI.sendMessage(
            `op ${operator_number} release ${value}`
        );
    });

    attachSliderListener(operator.volume, (value) => {
        window.electronAPI.sendMessage(`op ${operator_number} volume ${value}`);
    });

    operator.lfo_link.lfo1.addEventListener('click', () => {
        operator.lfo_link.lfo1.classList.toggle('active');
        window.electronAPI.sendMessage(
            `op ${operator_number} lfo1 toggle ${operator.lfo_link.lfo1.classList.contains(
                'active'
            )}`
        );
    });

    operator.lfo_link.lfo2.addEventListener('click', () => {
        operator.lfo_link.lfo2.classList.toggle('active');
        window.electronAPI.sendMessage(
            `op ${operator_number} lfo2 toggle ${operator.lfo_link.lfo2.classList.contains(
                'active'
            )}`
        );
    });

    return operator;
};

const initLFO = (lfo_number: number): LFO => {
    const lfo = {
        wave_form: {
            wave_value: document.getElementById(
                `wave-type-lfo-${lfo_number}`
            ) as HTMLSpanElement,
            left_button: document.getElementById(
                `left-wave-selector-lfo-${lfo_number}`
            ) as HTMLButtonElement,
            right_button: document.getElementById(
                `right-wave-selector-lfo-${lfo_number}`
            ) as HTMLButtonElement,
        },
        rate: document.getElementById(
            `rate-knob-lfo-${lfo_number}`
        ) as HTMLInputElement,
        amount: document.getElementById(
            `amount-knob-lfo-${lfo_number}`
        ) as HTMLInputElement,
    } as LFO;

    lfo.wave_form.right_button?.addEventListener('click', () => {
        const idx = WAVE_FORMS.findIndex(
            (w) => w.text === lfo.wave_form.wave_value.textContent
        );
        lfo.wave_form.wave_value.setHTMLUnsafe(
            WAVE_FORMS[(idx + 1) % WAVE_FORMS.length].text
        );
        window.electronAPI.sendMessage(
            `lfo ${lfo_number} waveform ${
                WAVE_FORMS[(idx + 1) % WAVE_FORMS.length].value
            }`
        );
    });

    lfo.wave_form.left_button?.addEventListener('click', () => {
        const idx = WAVE_FORMS.findIndex(
            (w) => w.text === lfo.wave_form.wave_value.textContent
        );
        lfo.wave_form.wave_value.setHTMLUnsafe(
            WAVE_FORMS[idx - 1 < 0 ? idx + WAVE_FORMS.length - 1 : idx - 1].text
        );
        window.electronAPI.sendMessage(
            `lfo ${lfo_number} waveform ${
                WAVE_FORMS[idx - 1 < 0 ? idx + WAVE_FORMS.length - 1 : idx - 1]
                    .value
            }`
        );
    });

    attachKnobListener(lfo.rate, (value) => {
        window.electronAPI.sendMessage(`lfo ${lfo_number} rate ${value}`);
    });

    attachKnobListener(lfo.amount, (value) => {
        window.electronAPI.sendMessage(`lfo ${lfo_number} amount ${value}`);
    });

    return lfo;
};

const attachKnobListener = (
    knob: HTMLElement | null,
    onChange?: (value: number) => void,
    minAngle = MIN_ANGLE,
    maxAngle = MAX_ANGLE
) => {
    if (knob === null) return;

    let rotating = false;
    let currentAngle = 40;
    let normalizedValue = 0;

    knob.addEventListener('mousedown', (e) => {
        const xpos = knob.getBoundingClientRect().x;
        const ypos = knob.getBoundingClientRect().y;
        const width = knob.clientWidth;
        const height = knob.clientHeight;
        const xmargin = width * 0.4;
        const ymargin = height * 0.4;
        e.preventDefault();
        if (
            e.x <= xpos + width + xmargin &&
            e.x >= xpos - xmargin &&
            e.y <= ypos + height + ymargin &&
            e.y >= ypos - ymargin
        ) {
            rotating = true;
        }
    });

    document.addEventListener('mousemove', (e) => {
        if (rotating) {
            let newAngle = Math.floor(calculateDegree(e, knob));
            newAngle = Math.max(minAngle, Math.min(maxAngle, newAngle));

            // disallows shifts from 320 to 40
            if (Math.abs(newAngle - currentAngle) > 200) {
                return;
            }

            currentAngle = newAngle;
            knob.style.transform = `rotate(${currentAngle}deg)`;

            // from 0 to 1
            normalizedValue = (currentAngle - minAngle) / (maxAngle - minAngle);
            knob.style.setProperty(
                '--knob-after-opacity',
                `${normalizedValue * 100}%`
            );
        }
    });

    document.addEventListener('mouseup', () => {
        if (rotating) {
            onChange?.(normalizedValue);
            rotating = false;
        }
    });
};

const calculateDegree = (e: MouseEvent, knob: HTMLElement): number => {
    const rect = knob.getBoundingClientRect();
    const knobCenterX = rect.left + rect.width / 2;
    const knobCenterY = rect.top + rect.height / 2;
    const deltaX = e.clientX - knobCenterX;
    const deltaY = e.clientY - knobCenterY;

    const rad = Math.atan2(deltaY, deltaX);

    const deg = rad * (180 / Math.PI) - 90; // Adjust offset

    return deg < 0 ? deg + 360 : deg;
};

const attachSliderListener = (
    slider: HTMLInputElement,
    onChange?: (normalized: number) => void
) => {
    if (!slider) return;

    const minValue = isNaN(Number(slider.min)) ? 0 : Number(slider.min);
    const maxValue = isNaN(Number(slider.max)) ? 100 : Number(slider.max);

    slider.addEventListener('mouseup', () => {
        const value = isNaN(Number(slider.value)) ? 75 : Number(slider.value);
        const normalized = (value - minValue) / (maxValue - minValue); // Normalize to [0, 1]
        onChange?.(normalized);
    });
};

window.onload = () => {
    document.getElementById('body-container');
    globalVariables.menu_dialog = new MenuSelectionDialog(
        document.getElementById('menu-dialog') as HTMLDialogElement
    );

    for (let operator_number = 1; operator_number != 5; ++operator_number) {
        globalVariables.operators[`${operator_number}` as '1'] =
            initOperator(operator_number);
    }

    for (let lfo_number = 1; lfo_number != 3; ++lfo_number) {
        globalVariables.lfos[`${lfo_number}` as '1'] = initLFO(lfo_number);
    }

    const keyboardGrabButton = document.getElementById('keyboard-activator');
    const high_pass_knob = document.getElementById('high-pass-knob');
    const low_pass_knob = document.getElementById('low-pass-knob');
    const syncButton = document.getElementById(
        'light-up-button-lfo1'
    ) as HTMLButtonElement;
    const main_volume: HTMLInputElement = document.getElementById(
        'main-volume'
    ) as HTMLInputElement;

    let on_off = false;

    if (syncButton) {
        syncButton.addEventListener('click', () => {
            syncButton.classList.toggle('active');
            on_off = !on_off;
            console.log(on_off);
        });
    }

    if (high_pass_knob)
        attachKnobListener(high_pass_knob, (value: number) => {
            window.electronAPI.sendMessage(`high-pass ${value}`);
        });

    if (low_pass_knob)
        attachKnobListener(low_pass_knob, (value: number) => {
            window.electronAPI.sendMessage(`low-pass ${value}`);
        });

    if (main_volume)
        attachSliderListener(main_volume, (value: number) => {
            window.electronAPI.sendMessage(`main-volume-${value}`);
        });

    keyboardGrabButton?.addEventListener('click', () => {
        window.electronAPI.grabKeyboard();
    });
};

window.electronAPI.endKeyboardGrab(() => {
    console.log('ended keyboard grabbing');
});

window.electronAPI.selectDeviceName((deviceNames: string[]) => {
    globalVariables.menu_dialog?.addDialog(
        'Select Device',
        deviceNames,
        (idx: number) => {
            window.electronAPI.sendMessage(`selected device index ${idx}`);
        },
        'idx'
    );
});
