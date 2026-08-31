# FT-INT-05 — Procédures détaillées

## 1. Principes communs

- Utiliser le mapping V1 courant.
- Capturer les valeurs avant et après le stimulus.
- Ne prononcer un verdict FT-INT sur un stimulus B5 que si la commande nécessaire est exploitable ; les erreurs du moteur sont analysées dans FT-CMD.
- Les essais d'échec autotest et d'ACK nécessitent une injection déterministe documentée.
- Les contrôles TRACE_ONLY sont enregistrés sans transformer une proximité plausible en exigence normative.

---

## TT-INT-B05B07-001 — Publication d'un autotest réussi

### Objectif

Vérifier qu'un autotest B5 dont l'issue réelle est maîtrisée comme réussie est publié dans B7.

### Préconditions

- capteur dans un état compatible avec SELFTEST ;
- B7 lisible ;
- moyen d'essai permettant de garantir ou d'établir l'issue OK de l'autotest ;
- conditions B5 nécessaires satisfaites.

### Procédure

1. Lire et enregistrer `B7.selftest_status`, `selftest_result_code` et `selftest_detail`.
2. Déclencher SELFTEST via B5 selon la procédure FT-CMD applicable.
3. Vérifier que l'exécution B5 est exploitable pour le scénario FT-INT.
4. Attendre l'achèvement fonctionnel de l'autotest selon les indications normatives disponibles, sans inventer un délai fixe.
5. Relire les trois champs B7.
6. Enregistrer la chronologie des observations.

### PASS

- l'issue maîtrisée comme réussie est observable dans B7 par un `selftest_status` final compatible avec OK ;
- les champs résultat/détail sont lisibles et appartiennent aux domaines V1, sans exiger une signification supplémentaire non normée.

### FAIL

B5 indique une exécution exploitable et l'issue réelle de l'autotest est établie comme réussie, mais B7 ne publie pas un état final compatible avec OK.

### INCONCLUSIVE

L'issue réelle de l'autotest ne peut pas être établie ou la commande B5 n'est pas exploitable.

---

## TT-INT-B05B07-002 — Publication d'un autotest en échec

### Objectif

Vérifier la publication B7 d'un échec d'autotest déterministement provoqué.

### Préconditions

- moyen d'injection sûr et documenté permettant de provoquer un échec d'autotest sans ambiguïté ;
- conditions B5 nécessaires satisfaites.

### Procédure

1. Installer l'injection de défaut prévue par le banc.
2. Lire les champs SELFTEST B7 avant stimulus.
3. Déclencher SELFTEST via B5.
4. Confirmer que le stimulus B5 est exploitable.
5. Attendre l'achèvement fonctionnel.
6. Relire `selftest_status`, `selftest_result_code`, `selftest_detail`.
7. Retirer l'injection conformément à la procédure de banc.

### PASS

L'échec réel provoqué est publié par un `selftest_status` final compatible avec échec.

### FAIL

L'échec réel est établi, B5 est exploitable, mais B7 publie un état final incompatible avec cet échec.

### INCONCLUSIVE

Le banc ne garantit pas l'échec ou l'exécution B5 n'est pas exploitable.

### Limite

Le test n'invente pas de table cause injectée → `selftest_result_code`/`selftest_detail` si la V1 ne la définit pas.

---

## TT-INT-B01B05B07-001 — Acquittement sans disparition de la cause

### Objectif

Vérifier qu'un ACK B5 ne fait pas disparaître une cause de défaut qui reste présente.

### Préconditions

- moyen d'injection d'un défaut V1 observable et maintenable ;
- défaut effectivement visible dans au moins l'observable normatif concerné avant ACK ;
- commande ACK disponible dans les conditions prévues par B5.

### Procédure

1. Injecter le défaut contrôlé.
2. Attendre qu'il soit observable dans B1 et/ou B7 selon les champs normativement concernés par ce défaut.
3. Capturer B1 et B7 avant ACK.
4. Maintenir physiquement/logiquement la cause du défaut.
5. Exécuter ACK via B5.
6. Confirmer que l'exécution B5 est exploitable.
7. Relire B1 et B7 sans retirer la cause.
8. Enregistrer les champs avant/après.
9. Retirer ensuite l'injection selon la procédure du banc.

### PASS

La cause restant présente, l'observable normatif qui matérialise cette cause ne disparaît pas du seul fait de l'ACK.

### FAIL

La cause est maintenue et établie, mais l'ACK fait disparaître l'observable normatif de la cause contrairement à la règle V1.

### INCONCLUSIVE

Le défaut n'est pas injecté ou maintenu de façon déterministe, ou l'ACK n'est pas exploitable.

### Limite

Aucune égalité exhaustive entre les bitfields B1 et B7 n'est requise.

---

## TT-INT-B02B07-001 — Base temporelle du dernier défaut

### Objectif

Vérifier que `B7.last_fault_timestamp` est exprimé dans la base temporelle B2.

### Préconditions

- B2 fournit une base temporelle exploitable ;
- possibilité de provoquer ou d'identifier un nouveau défaut significatif mettant à jour `last_fault_timestamp` ;
- événement suffisamment identifiable pour établir une fenêtre temporelle.

### Procédure

1. Lire B2 et `B7.last_fault_timestamp` avant l'événement.
2. Définir une borne temporelle B2 avant injection.
3. Provoquer un défaut contrôlé significatif.
4. Observer la mise à jour de `last_fault_timestamp`.
5. Lire une borne temporelle B2 après l'événement.
6. Vérifier que le timestamp B7 appartient à la même base et est compatible avec la fenêtre de l'événement.

### PASS

Le timestamp B7 est cohérent avec la base B2 et la fenêtre temporelle du défaut.

### FAIL

Le timestamp mis à jour est manifestement exprimé dans une autre base ou incompatible avec la fenêtre B2 établie.

### INCONCLUSIVE

Aucun nouveau défaut significatif ne peut être provoqué/identifié ou la fenêtre temporelle ne peut être établie.

### Limite

Aucune égalité avec une lecture B2 séparée ni tolérance arbitraire n'est imposée.

---

## TT-INT-B01B07-001 — Observables dupliqués B1/B7

### Statut

TRACE_ONLY — contrôle croisé non bloquant.

### Objectif

Documenter les observables présents dans les deux blocs sans créer d'oracle absent de la V1.

### Procédure

1. Lire dans une séquence aussi rapprochée que raisonnablement possible les champs d'uptime B1 et B7.
2. Lire les champs de cause de reset B1 et B7.
3. Lire les températures internes B1 et B7.
4. Enregistrer valeurs, ordre de lecture et timestamps de la campagne de test.
5. Signaler toute divergence notable comme observation à analyser, sans verdict FAIL FT-INT sur la seule base de cette divergence.

### Résultat

Le test produit une trace comparative. Il ne possède pas de critère PASS/FAIL inter-blocs strict en V1.

### Délégations

- monotonie d'uptime : FT-BLK ;
- domaines/codes : FT-LIM ;
- persistance/reset : FT-PER.

---

## 2. Relations explicitement non transformées en tests PASS/FAIL

- `B1.system_status` ↔ `B7.system_health_status` : NOT_DEFINED ;
- bitfields défaut B1 ↔ B7 : NOT_DEFINED ;
- état stockage B6 ↔ diagnostic B7 : NOT_DEFINED.

Ces absences d'oracle doivent rester visibles dans la matrice de couverture et dans l'audit final FT-INT.