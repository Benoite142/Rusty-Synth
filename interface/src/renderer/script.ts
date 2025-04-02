const MIN_ANGLE = 40; // Min rotation (Bottom-left)
const MAX_ANGLE = 320; // Max rotation (Bottom-right)

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

  public addDialog = <I>(selectionTitle: string, items: I[], callBack: (value: I | number) => void, returnType: 'idx' | 'value'): void => {
    // would only want to alter the options and title
    const options = items.map((item, idx) => `<option value="${idx}">${item}</option>`).join('');
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
    document.getElementById('confirm-btn').addEventListener('click', (e) => {
      e.preventDefault();
      callBack(returnType === 'idx' ? selector.value as any as number : items[Number(selector.value)])
      this.dialog.close();
    });
  }
}

type Operator = {
  wave_form: HTMLSelectElement | undefined,
  attack: HTMLInputElement | undefined,
  decay: HTMLInputElement | undefined,
  sustain: HTMLInputElement | undefined,
  release: HTMLInputElement | undefined,
}

type GlobalVariables = {
  menuDialog: MenuSelectionDialog | undefined,
  operators: {
    '1': Operator | undefined,
    '2': Operator | undefined,
    '3': Operator | undefined,
    '4': Operator | undefined,
  },
}

const globalVariables: GlobalVariables = {
  menuDialog: undefined, operators: { '1': undefined, '2': undefined, '3': undefined, '4': undefined }
}

const initOperator = (operator_number: number): Operator => {
  const operator = {
    wave_form: document.getElementById(`wave-form-selector-${operator_number}`) as HTMLSelectElement,
    attack: document.getElementById(`attack-knob-${operator_number}`) as HTMLInputElement,
    decay: document.getElementById(`decay-knob-${operator_number}`) as HTMLInputElement,
    sustain: document.getElementById(`sustain-knob-${operator_number}`) as HTMLInputElement,
    release: document.getElementById(`release-knob-${operator_number}`) as HTMLInputElement,
  }

  // operator.wave_form?.addEventListener('

  attachKnobListener(operator.attack, (value) => {
    window.electronAPI.sendMessage(`op ${operator_number} attack ${value}`);
  });

  attachKnobListener(operator.decay, (value) => {
    window.electronAPI.sendMessage(`op ${operator_number} decay ${value}`);
  });

  attachKnobListener(operator.sustain, (value) => {
    window.electronAPI.sendMessage(`op ${operator_number} sustain ${value}`);
  });

  attachKnobListener(operator.release, (value) => {
    window.electronAPI.sendMessage(`op ${operator_number} release ${value}`);
  });

  return operator;
}

const attachKnobListener = (
  knob: HTMLElement | null,
  onChange?: (value: number) => void,
  minAngle = MIN_ANGLE,
  maxAngle = MAX_ANGLE,
) => {
  if (knob === null) return;

  let rotating = false;
  let currentAngle = 40;
  let normalizedValue = 0;

  knob.addEventListener("mousedown", (e) => {
    e.preventDefault();
    console.log(e.x, knob.getBoundingClientRect().x + knob.clientWidth
      , knob.getBoundingClientRect().x
      , e.y, knob.getBoundingClientRect().y + knob.clientHeight
      , knob.getBoundingClientRect().y);
    if (e.x <= knob.getBoundingClientRect().x + knob.clientWidth
      && e.x >= knob.getBoundingClientRect().x
      && e.y <= knob.getBoundingClientRect().y + knob.clientHeight
      && e.y >= knob.getBoundingClientRect().y) {
      rotating = true;
    }
  });

  document.addEventListener("mousemove", (e) => {
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
        "--knob-after-opacity",
        `${normalizedValue * 100}%`
      );
    }
  });

  document.addEventListener("mouseup", () => {
    if (rotating) {
      onChange?.(normalizedValue);
      rotating = false;
    }
  });
}

const calculateDegree = (e: MouseEvent, knob: HTMLElement): number => {
  const rect = knob.getBoundingClientRect();
  const knobCenterX = rect.left + rect.width / 2;
  const knobCenterY = rect.top + rect.height / 2;
  const deltaX = e.clientX - knobCenterX;
  const deltaY = e.clientY - knobCenterY;

  const rad = Math.atan2(deltaY, deltaX);

  const deg = rad * (180 / Math.PI) - 90; // Adjust offset

  return deg < 0 ? deg + 360 : deg;
}

const attachSliderListener = (
  slider: HTMLInputElement,
  onChange?: (normalized: number) => void
) => {
  if (!slider) return;

  const minValue = isNaN(Number(slider.min)) ? 0 : Number(slider.min);
  const maxValue = isNaN(Number(slider.max)) ? 100 : Number(slider.max);

  slider.addEventListener("input", () => {
    const value = isNaN(Number(slider.value)) ? 75 : Number(slider.value);
    const normalized = (value - minValue) / (maxValue - minValue); // Normalize to [0, 1]
    onChange?.(normalized);
  });
}

window.onload = () => {
  globalVariables.menuDialog = new MenuSelectionDialog(document.getElementById('menu-dialog') as HTMLDialogElement);

  for (let operator_number = 1; operator_number != 5; ++operator_number) {
    globalVariables.operators[`${operator_number}` as '1'] = initOperator(operator_number);
  }


  const keyboardGrabButton = document.getElementById('grab-keyboard-button');
  const high_pass_knob = document.getElementById("high-pass-knob");
  const low_pass_knob = document.getElementById("low-pass-knob");
  const syncButton = document.getElementById(
    "light-up-button-lfo1"
  ) as HTMLButtonElement;
  const main_volume: HTMLInputElement = document.getElementById(
    "main-volume"
  ) as HTMLInputElement;

  let on_off = false;

  if (syncButton) {
    syncButton.addEventListener("click", () => {
      syncButton.classList.toggle("active");
      on_off = !on_off;
      console.log(on_off);
    });
  }

  if (high_pass_knob)
    attachKnobListener(high_pass_knob, (value) => {
      // console.log("Knob-Value", value);
    });

  if (low_pass_knob)
    attachKnobListener(low_pass_knob, (value) => {
      // console.log("Knob-Value", value);
    });


  if (main_volume)
    attachSliderListener(main_volume, (normalizeValue) => {
      // console.log("nromalizedvalue", normalizeValue);
    });

  keyboardGrabButton?.addEventListener('click', () => {
    window.electronAPI.grabKeyboard();
  });
};

window.electronAPI.endKeyboardGrab(() => {
  console.log('ended keyboard grabbing');
});


window.electronAPI.selectDeviceName((deviceNames: string[]) => {
  globalVariables.menuDialog?.addDialog('Select Device', deviceNames, (idx: number) => {
    window.electronAPI.sendMessage(`selected device index ${idx}`);
  }, 'idx');
});
