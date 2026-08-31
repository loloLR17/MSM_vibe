\# FT-STR-05 — Fiche de spécification  

\## Registres réservés et sentinelles structurelles



---



\## 1. Identification



\- \*\*ID\*\* : FT-STR-05  

\- \*\*Nom\*\* : Réservés et sentinelles  

\- \*\*Famille parente\*\* : FT-STR  

\- \*\*Criticité\*\* : P0 (bloquant)



---



\## 2. Objectif



Valider que les zones non utilisées ou spéciales :



\- respectent les conventions (ex : 0),

\- sont stables,

\- ne véhiculent aucune information parasite.



---



\## 3. Périmètre



\### Inclus



\- registres réservés

\- valeurs imposées (ex : ID = 0)

\- zones non utilisées

\- stabilité de ces zones



\### Exclus



\- logique métier des valeurs

\- états dynamiques



---



\## 4. Références



\- "Registres réservés = 0"

\- "ID = 0 → non renseigné"



---



\## 5. Règles



Un registre réservé est conforme si :



\- valeur = 0

\- stable dans le temps

\- jamais utilisé implicitement



---



\## 6. Préconditions



\- FT-STR-02 validée



---



\## 7. Résultats attendus



\- zones neutres

\- absence de bruit

\- cohérence totale



---



\## 8. Risques



| Risque | Impact |

|---|---|

| Valeurs non nulles | ambiguïté |

| Valeurs instables | non fiabilité |

| fuite mémoire | comportement caché |

| mauvaise sentinelle | erreur logique |



---



\## 9. Cas de test



\### Nominal

\- lecture réservés → 0



\### Limites

\- lecture répétée

\- lecture multi-blocs



\### Erreurs

\- valeur non nulle

\- variation



---



\## 10. Points critiques



\- zones mémoire non initialisées

\- valeurs fantômes

\- confusion entre "0" et "valide"



---



\## 11. Ambiguïtés



\- différence entre :

&nbsp; - réservé

&nbsp; - non renseigné

&nbsp; - invalide

&nbsp; - absent



---



\## 12. Réussite



\- 100% réservés à 0

\- stabilité totale



---



\## 13. Échec



\- valeur non nulle

\- instabilité

\- comportement implicite



---



\## 14. Anomalies



| Type | Exemple |

|---|---|

| BLOQUANTE | valeur ≠ 0 |

| MAJEURE | instabilité |

| SPEC | doctrine floue |



---



\## 15. Dépendances



\### Amont

\- FT-STR-02



---



\## 16. Ordre



1\. Identifier zones réservées

2\. Lire valeurs

3\. Vérifier nullité

4\. Vérifier stabilité

5\. Vérifier isolation



---



\## 17. Livrables



\- dump registres réservés

\- rapport conformité



---



\## 18. Maturité



\- aucune valeur parasite

\- comportement totalement neutre

\- doctrine claire



---

