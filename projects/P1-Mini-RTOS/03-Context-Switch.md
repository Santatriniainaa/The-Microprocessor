# 3. Changement de Contexte (Context Switch)

> **Auteur** : Santatriniaina RAZAFINDRAKOTO | **Date** : 20 mars 2026

[Retour a l'index projet](./00-INDEX-PROJET.md) | [<< Scheduler](./02-Scheduler.md) | [SysTick >>](./04-SysTick-Timer.md)

---

## 3.1 Principe

Le changement de contexte est le **coeur du RTOS**. Il consiste a :
1. **Sauvegarder** les registres de la tache courante sur sa pile
2. **Mettre a jour** le pointeur de pile dans le TCB
3. **Selectionner** la tache suivante
4. **Restaurer** les registres de la nouvelle tache depuis sa pile

## 3.2 Registres a Sauvegarder

Lors d'une exception, le CPU sauvegarde **automatiquement** 8 registres :

| Registre | Sauvegarde |
|----------|-----------|
| R0-R3, R12, LR, PC, xPSR | **Automatique** (par le hardware) |
| R4-R11 | **Manuelle** (par le code du handler) |

> On doit donc sauvegarder **manuellement** R4-R11 dans le handler PendSV.

## 3.3 Handler PendSV (en assembleur)

```asm
    .syntax unified
    .thumb

    .global PendSV_Handler
    .thumb_func
PendSV_Handler:
    // --- Sauvegarder le contexte de la tache courante ---
    // A l'entree, le CPU a deja sauvegarde R0-R3,R12,LR,PC,xPSR
    // sur la pile de la tache courante (PSP)

    mrs  r0, psp              // r0 = PSP (pile de la tache courante)
    subs r0, r0, #32          // reserve 8 mots (R4-R11)
    stm  r0!, {r4-r7}         // sauvegarder R4-R7
    mov  r4, r8
    mov  r5, r9
    mov  r6, r10
    mov  r7, r11
    stm  r0!, {r4-r7}         // sauvegarder R8-R11 (via R4-R7)
    subs r0, r0, #32          // repositionner r0 au debut

    // --- Sauvegarder SP dans le TCB courant ---
    ldr  r1, =current_task
    ldr  r2, [r1]             // r2 = index tache courante
    ldr  r3, =tcb             // r3 = adresse du tableau TCB
    lsls r2, r2, #2           // r2 = offset (index * 4)
    // Note: ajuster selon sizeof(TCB_t)
    str  r0, [r3, r2]         // tcb[current].sp = r0

    // --- Selectionner la tache suivante ---
    ldr  r0, [r1]             // r0 = current_task
    adds r0, r0, #1           // r0 = current_task + 1
    ldr  r4, =NUM_TASKS
    ldr  r4, [r4]
    cmp  r0, r4
    blt  no_wrap
    movs r0, #0               // wraparound a 0
no_wrap:
    str  r0, [r1]             // current_task = nouvelle tache

    // --- Restaurer le contexte de la nouvelle tache ---
    lsls r0, r0, #2
    ldr  r2, [r3, r0]         // r2 = tcb[new].sp

    ldm  r2!, {r4-r7}         // restaurer R4-R7
    mov  r8, r4
    mov  r9, r5
    mov  r10, r6
    mov  r11, r7
    ldm  r2!, {r4-r7}         // restaurer les vrais R4-R7 (sauvegardes apres)
    // Note: l'ordre peut varier selon l'implementation

    msr  psp, r2              // mettre a jour PSP

    // --- Retour d'exception ---
    ldr  r0, =0xFFFFFFFD      // EXC_RETURN: Thread mode, PSP
    bx   r0
```

> **Note** : Sur Cortex-M0, on ne peut pas faire `PUSH {R4-R11}` directement car STM sur M0 ne supporte que R0-R7. Il faut donc copier R8-R11 dans R4-R7 temporairement.

## 3.4 Diagramme du Changement de Contexte

```
SysTick tick !
     |
     v
+--------------------+     +---------------------+
| SysTick_Handler    |     |                     |
| - Set PendSV pend  |---->| PendSV_Handler      |
+--------------------+     |                     |
                           | 1. MRS R0, PSP      |
  Task A en cours          | 2. PUSH R4-R11      |
  +-----------+            |    sur pile Task A   |
  | R4-R11    |            | 3. Sauve SP → TCB[A] |
  | R0-R3,LR..|            | 4. current = next    |
  +-----------+            | 5. SP ← TCB[B].sp   |
                           | 6. POP R4-R11       |
  Task B reprend           |    depuis pile Task B|
  +-----------+            | 7. MSR PSP, SP      |
  | R4-R11    |            | 8. BX EXC_RETURN    |
  | R0-R3,LR..|            +---------------------+
  +-----------+                    |
       ^                          |
       +---- CPU restaure auto ----+
              R0-R3,R12,LR,PC,xPSR
```

---

[<< Scheduler](./02-Scheduler.md) | [SysTick Timer >>](./04-SysTick-Timer.md)
