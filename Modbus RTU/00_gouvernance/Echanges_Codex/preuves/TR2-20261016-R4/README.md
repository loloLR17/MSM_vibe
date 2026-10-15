# R4 — checkpoint logiciel et préflight matériel

136tests et cross-build réussis ; charge UART renforcée et ASan ciblé conservés. Aucun essai physique de coexistence encore effectué à ce checkpoint. Seuls GDB read-only, halt/detach, lecture Flash/FRAM et RAM de sauvegarde exécutés, sans reset/flash/write FRAM/SD/RS485. Deux lectures complètes vérifiées de Flash et FRAM, binaires gardés localement dans reports/TR2-r4/hardware ; empreintes dans les summaries.

Deux diagnostics GDB préparatoires ont échoué avant lecture FRAM : frame de qualification absente (le firmware attend dans CommissioningReadOnly), puis nom de buffer SD absent. Inspection du stack et lecture via storage local réel ont ensuite réussi. Aucune erreur masquée, aucun essai physique répété.
