let counterSpan = document.getElementById("counter");

function attachKnobListener(
  knob: HTMLElement | null,
  minAngle = 40,
  maxAngle = 320,
  onChange?: (value: number) => void
) {
  if (!knob) return;

  let rotating = false;
  let currentAngle = 40;

  knob.addEventListener("mousedown", (e) => {
    rotating = true;
    e.preventDefault();
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
      const normalizeValue = (currentAngle - minAngle) / (maxAngle - minAngle);
      knob.style.setProperty(
        "--knob-after-opacity",
        `${normalizeValue * 100}%`
      );
      onChange?.(normalizeValue);
    }
  });

  document.addEventListener("mouseup", () => {
    rotating = false;
  });
}

function calculateDegree(e: MouseEvent, knob: HTMLElement): number {
  const rect = knob.getBoundingClientRect();
  const knobCenterX = rect.left + rect.width / 2;
  const knobCenterY = rect.top + rect.height / 2;
  const deltaX = e.clientX - knobCenterX;
  const deltaY = e.clientY - knobCenterY;

  const rad = Math.atan2(deltaY, deltaX);

  const deg = rad * (180 / Math.PI) - 90; // Adjust offset

  return deg < 0 ? deg : deg + 360;
}

function attachSliderListener(
  slider: HTMLInputElement,
  onChange?: (normalized: number) => void
) {
  if (!slider) return;

  const minValue = isNaN(Number(slider.min)) ? 0 : Number(slider.min);
  const maxValue = isNaN(Number(slider.max)) ? 100 : Number(slider.max);

  slider.addEventListener("input", () => {
    const value = isNaN(Number(slider.value)) ? 75 : Number(slider.value);
    const normalized = (value - minValue) / (maxValue - minValue); // Normalize to [0, 1]
    onChange?.(normalized);
  });
}

// button is not on document before on load
// could be used to initialise global values
window.onload = () => {
  const actionButton = document.getElementById("action-button");
  const valueFromMainSlot = document.getElementById("value-from-main");
  const high_pass_knob = document.getElementById("high-pass-knob");
  const attack_knob_op1 = document.getElementById("attack-knob-op1");
  const low_pass_knob = document.getElementById("low-pass-knob");
  const syncButton = document.getElementById(
    "light-up-button-lfo1"
  ) as HTMLButtonElement;
  const main_volume: HTMLInputElement = document.getElementById(
    "main-volume"
  ) as HTMLInputElement;

  const MIN_ANGLE = 40; // Min rotation (Bottom-left)
  const MAX_ANGLE = 320; // Max rotation (Bottom-right)
  // chances are it did not get picked up initially
  let on_off = false;

  if (syncButton) {
    syncButton.addEventListener("click", () => {
      syncButton.classList.toggle("active");
      on_off = !on_off;
      console.log(on_off);
    });
  }

  if (!counterSpan) {
    counterSpan = document.getElementById("counter");
  }
  if (high_pass_knob)
    attachKnobListener(high_pass_knob, MIN_ANGLE, MAX_ANGLE, (value) => {
      console.log("Knob-Value", value);
    });

  if (low_pass_knob)
    attachKnobListener(low_pass_knob, MIN_ANGLE, MAX_ANGLE, (value) => {
      console.log("Knob-Value", value);
    });

  if (attack_knob_op1)
    attachKnobListener(attack_knob_op1, MIN_ANGLE, MAX_ANGLE, (value) => {
      console.log("Knob-Value", value);
    });

  if (main_volume)
    attachSliderListener(main_volume, (normalizeValue) => {
      console.log("nromalizedvalue", normalizeValue);
    });
  actionButton?.addEventListener("click", async () => {
    const valueFromMain = await window.electronAPI.sendToMainProcess(
      "value from ui"
    );
    valueFromMainSlot.innerText = valueFromMain;
  });
};

window.electronAPI.onUpdateCounter((value: number) => {
  counterSpan.innerText = (Number(counterSpan.innerText) + value).toString();
});
