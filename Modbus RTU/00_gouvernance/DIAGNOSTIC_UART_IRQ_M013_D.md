# M013-D — procédure d'accès STM32 corrigée

11 octobre 2026 ; code évalué 09792c8dba98cd3959458f3148d4fae63a9584c9. Preuves et limites détaillées dans Echanges_Codex/archives/TR2-20261011-M013-D-UART-IRQ-ROOT-CAUSE/RAPPORT.md.

Sur STM32U575/ST-LINK003500463235510B37333439 et CubeProgrammer Windows2.23.0, une lecture HOTPLUG reproduit ISER=0/ISPR=FFFFFFFF, confirmé par le cœur. GDB seul préserve les IRQ49/66 activées. TrustZone est désactivé ; pas de correction firmware NVIC justifiée. Le mécanisme interne du logiciel ST reste non isolé.

Procédure vérifiée en D, sous permissions de chaque mission et chaîne qualifiée :

1. Préserver l'état RAM et les preuves avant tout reset/reflash.
2. Faire les opérations CLI nécessaires (backup vérifié, flash standard, verify/readback).
3. Terminer par reset logiciel GDB, detach et contrôle passif CPU/SWD du NVIC ; lecture Flash via GDB possible et vérifiée ici sans dérèglement.
4. Ne pas reconnecter CubeProgrammer au MCU après ce contrôle sain avant un essai. Si nouvel accès CLI indispensable, considérer l'état IRQ non fiable et refaire la clôture GDB autorisée après préservation.

État laissé D : diagnostic isolé unité1, observerON, **TXOFF**, MCU en exécution, IRQ49/66 activées, UART sans RXNE/ORE, PG4 bas. Aucun nouvel essai HP ni émission autorisé ici. L'observateur RAM compte IRQ/HAL/file/RTU/TX et conserve snapshots/trames sans log RS485. Tests hôte5/5 et cross-build réussis ; aucune qualification physique Modbus.

Une prochaine mission devra autoriser un candidat TXON et un unique essai instrumenté. Avant essai : image exacte, ready1/polling progressant, IRQ49/66 CPU et SWD corrects, RX armé sans erreur résiduelle, PG4 bas. Pendant échange : aucun halt ; capture réponse/CRC et progression des compteurs, capture DE/TC/retourRX si instrumentation disponible. Pas de répétition des contrôles physiques acquis ni changement de montage implicite.
