# FT-PER-02 — Cas de test détaillé

## TT-PER-B00-001 — Persistance du device_id après RESET SOFTWARE

**Classification d'exécution : `CONDITIONAL`.**

### Objectif

Vérifier que le `device_id` normativement persistent conserve exactement sa valeur à travers un redémarrage logiciel contrôlé.

### Sources normatives

- Bloc 0 §5 et §7 ;
- FT-PER-01 pour la frontière de reboot ;
- FT-STR-03 pour la reconstruction uint32 MSW puis LSW.

### Préconditions

- Bloc 0 lisible ;
- moyen permettant d'exécuter un RESET SOFTWARE dans les conditions normatives ;
- FT-PER-01 ou preuve équivalente permettant d'établir que la frontière de reboot a réellement été franchie ;
- lecture du `device_id` effectuée comme uint32 cohérent.

### Étapes

1. lire `device_id_msw` et `device_id_lsw` dans une lecture cohérente et reconstruire `device_id_before` ;
2. enregistrer les mots bruts et la valeur uint32 ;
3. provoquer un RESET SOFTWARE conforme à la procédure FT-PER-01 / FT-CMD-07 ;
4. attendre uniquement le retour observable de l'interface, sans imposer de délai V1 ;
5. confirmer que le reboot est établi par le scénario FT-PER-01 ;
6. relire `device_id_msw` et `device_id_lsw` et reconstruire `device_id_after` ;
7. comparer bit à bit les valeurs avant/après.

### Résultat attendu

`device_id_after == device_id_before`.

### PASS

- le reboot a été effectivement établi ;
- le `device_id` complet après reboot est strictement identique à celui relevé avant reboot.

### FAIL

Le reboot est établi et la valeur uint32 du `device_id` diffère avant/après.

### INCONCLUSIVE / NOT_EXECUTABLE

- le reboot ne peut pas être établi de façon fiable ;
- le RESET SOFTWARE ne peut pas être exécuté sur le moyen d'essai ;
- la lecture uint32 n'est pas exploitable.

### Limites

- ce test ne démontre pas l'unicité entre plusieurs capteurs ;
- ce test ne démontre pas la persistance des autres champs B0 ;
- ce test ne démontre pas la conservation après power cycle, watchdog, brown-out, reset externe ou firmware update ;
- aucune technologie de stockage n'est supposée.

---

## Cas volontairement non instanciés

Les champs `hardware_version`, versions firmware, `protocol_version`, `device_capabilities`, `serial_number` et `manufacturer` ne reçoivent pas de test PASS/FAIL de persistance post-reboot en V1 faute d'oracle explicite.

Ils peuvent éventuellement être relevés comme traces lors de la campagne, mais une variation post-reboot ne doit pas être déclarée FAIL FT-PER-02 sur la seule base de leur caractère statique en fonctionnement normal.
